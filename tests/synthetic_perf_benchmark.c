/* SPDX-License-Identifier: GPL-2.0-only */
/* R0.5 synthetic performance baseline: CPU renderer and optional SDL presentation.
   Uses synthetic game fixtures only; no owned Wolfenstein data required. */
#include "engine/WL_RENDER.h"
#include "engine/WL_SAVE.h"
#include "engine/WL_FRAME_CLOCK.h"
#include "engine/ID_CA.h"
#include "engine/ID_VL.h"
#include "engine/ID_SD.h"
#include "engine/WL_PROFILE.h"
#include "platform/sdl_video.h"
#include "platform/sdl_audio.h"
#include "engine/WL_CONTROLS.h"
#include "engine/WL_MAIN.h"
#include "engine/WL_DISPLAY.h"
#include <SDL3/SDL.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#define CHECK(x) do { if(!(x)) { fprintf(stderr,"CHECK failed: %s (%s:%d) CA=%s SDL=%s\n",#x,__FILE__,__LINE__,CA_Error(),SDL_GetError()); exit(1); } } while(0)

#ifdef WOLF3D_BENCHMARK
wl_profile WL_Profile;
uint64_t WL_ProfileNow(void) { return SDL_GetTicksNS(); }
#endif

static int cmp_u64(const void *a,const void *b)
{
    uint64_t x=*(const uint64_t*)a,y=*(const uint64_t*)b; return (x>y)-(x<y);
}

static uint64_t rss_kb(void)
{
    struct rusage ru; if(getrusage(RUSAGE_SELF,&ru)!=0) return 0;
    /* Linux reports ru_maxrss in KiB. */
    return (uint64_t)ru.ru_maxrss;
}

extern statetype s_grdstand;

static void setup_empty_scene(void)
{
    CHECK(GameStart(0)); InitObjects(); CHECK(NewViewSize(19));
    memset(tilemap,0,sizeof(tilemap)); memset(areaMap,0,sizeof(areaMap));
    for(unsigned i=0;i<64;++i) tilemap[0][i]=tilemap[63][i]=tilemap[i][0]=tilemap[i][63]=1;
    for(unsigned x=1;x<=12;++x) tilemap[x][1]=tilemap[x][5]=1;
    for(unsigned y=1;y<=5;++y) tilemap[12][y]=1;
    player.x=2*TILEGLOBAL+32768; player.y=3*TILEGLOBAL+32768; player.tilex=2; player.tiley=3; player.angle=0;
    gamestate.weapon=gamestate.chosenweapon=gamestate.bestweapon=wp_pistol; gamestate.weaponframe=0;
    statcount=0; doornum=0; ResetPushwalls();
}

static void setup_populated_scene(void)
{
    setup_empty_scene();
    statcount=2;
    statobjlist[0]=(statobj_t){4,2,SPR_STAT_0,dressing,false};
    statobjlist[1]=(statobj_t){7,4,SPR_STAT_1,dressing,false};
    objtype *actor=SpawnActor(6,3,&s_grdstand); CHECK(actor);
    actor->angle=180; actor->dir=west; actor->active=true;
    doornum=1; tilemap[9][3]=128; doorobjlist[0]=(doorobj_t){9,3,0,true,dr_closed,0,0};
}

static void report_samples(const char *scene,const char *mode,unsigned w,unsigned h,unsigned n,uint64_t *samples)
{
    qsort(samples,n,sizeof(*samples),cmp_u64);
    uint64_t sum=0; for(unsigned i=0;i<n;++i) sum+=samples[i];
    uint64_t p50=samples[(n-1)/2];
    uint64_t p95=samples[(n-1)*95/100];
    uint64_t p99=samples[(n-1)*99/100];
    printf("CSV,%s,%s,%ux%u,%u,%u,%u,%" PRIu64 ",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%" PRIu64 "\n",
           scene,mode,w,h,w,h,n,sum,
           (double)samples[0]/1000000.0,
           (double)sum/(double)n/1000000.0,
           (double)p50/1000000.0,
           (double)p95/1000000.0,
           (double)p99/1000000.0,
           (double)samples[n-1]/1000000.0,
           rss_kb());
}

static void bench_cpu(const char *scene_name, void (*setup)(void))
{
    static const unsigned sizes[][2]={{320,200},{640,480},{1280,720},{1920,1080}};
    const unsigned warmup=20, iters=200;
    for(unsigned si=0;si<4;++si) {
        unsigned w=sizes[si][0],h=sizes[si][1];
        size_t bytes=(size_t)w*h;
        uint8_t *pixels=malloc(bytes); CHECK(pixels);
        int32_t *depth=malloc(sizeof(int32_t)*w); CHECK(depth);
        wl_render_target target; CHECK(WL_TargetInit(&target,pixels,bytes,w,h,w));
        setup();
        CHECK(GameDrawPaused());
        /* Warmup: stabilize caches/state. */
        for(unsigned i=0;i<warmup;++i) CHECK(WL_RenderFrameAspect(&target,depth,(size_t)w,true,w,h,NULL));
        uint64_t samples[200];
        for(unsigned i=0;i<iters;++i) {
            uint64_t t0=SDL_GetTicksNS();
            CHECK(WL_RenderFrameAspect(&target,depth,(size_t)w,true,w,h,NULL));
            samples[i]=SDL_GetTicksNS()-t0;
        }
        report_samples(scene_name,"cpu_render",w,h,iters,samples);
        free(depth); free(pixels);
        GameStop();
    }
}

static void bench_sdl(const char *data,const char *user)
{
    (void)data;
#ifdef WOLF3D_BENCHMARK
    static const unsigned sizes[][2]={{640,480},{1280,720},{1920,1080}};
    const unsigned warmup=20, iters=120;
    CHECK(VL_SetPalette(gamepal,768)); CHECK(VL_Startup());
    /* Audio is not required for SDL presentation overhead measurement. */
    Display_Connect(VL_DisplayGet,VL_DisplaySet);
    CHECK(Save_SetDirectory(user)); CHECK(DemoLoopStartup(0,false));
    int window_count; SDL_Window **windows=SDL_GetWindows(&window_count); CHECK(windows && window_count==1);
    (void)SDL_GetRenderer(windows[0]); SDL_free(windows);
    for(unsigned si=0;si<3;++si) {
        unsigned w=sizes[si][0],h=sizes[si][1];
        CHECK(DemoLoopStartGame(0)); setup_populated_scene();
        display_settings d; CHECK(VL_DisplayGet(&d));
        d.high_resolution=true; d.smooth=false; d.render_width=w; d.render_height=h;
        d.aspect=2; d.scale=0; d.width=1280; d.height=720; d.vsync=false;
        CHECK(VL_DisplaySet(&d));
        for(unsigned i=0;i<warmup;++i) { CHECK(!VL_PollQuit()); CHECK(GameDrawPaused()); CHECK(VW_UpdateScreen()); }
        uint64_t total[120], convert[120], upload[120], present[120];
        memset(&WL_Profile,0,sizeof(WL_Profile));
        for(unsigned i=0;i<iters;++i) {
            uint64_t c0=WL_Profile.convert,u0=WL_Profile.upload,p0=WL_Profile.present;
            uint64_t t0=SDL_GetTicksNS();
            CHECK(GameDrawPaused());
            CHECK(VW_UpdateScreen());
            total[i]=SDL_GetTicksNS()-t0;
            convert[i]=WL_Profile.convert-c0;
            upload[i]=WL_Profile.upload-u0;
            present[i]=WL_Profile.present-p0;
        }
        report_samples("populated","sdl_total",w,h,iters,total);
        report_samples("populated","sdl_convert",w,h,iters,convert);
        report_samples("populated","sdl_upload",w,h,iters,upload);
        report_samples("populated","sdl_present",w,h,iters,present);
        GameStop();
    }
    VL_Shutdown();
#else
    (void)data; (void)user;
    fprintf(stderr,"SDL presentation benchmark requires -DWOLF3D_BENCHMARK build\n");
#endif
}

int main(int argc,char **argv)
{
    if(argc<3) { fprintf(stderr,"usage: %s <data-dir> <user-dir> [--sdl]\n",argv[0]); return 2; }
    const char *data=argv[1], *user=argv[2];
    bool do_sdl=argc>3 && strcmp(argv[3],"--sdl")==0;
    printf("CSV_HEADER,scene,mode,resolution,width,height,iterations,total_ns,min_ms,mean_ms,p50_ms,p95_ms,p99_ms,max_ms,rss_kb\n");
    printf("META,compiler,%s\n",
#ifdef __clang__
           "clang"
#elif defined(__GNUC__)
           "gcc"
#else
           "unknown"
#endif
    );
    printf("META,compiler_version,%d.%d.%d\n",
#ifdef __GNUC__
           __GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__
#else
           0,0,0
#endif
    );
#ifdef __OPTIMIZE__
    printf("META,optimize,defined\n");
#else
    printf("META,optimize,not_defined\n");
#endif
    printf("META,data_dir,%s\n",data);
    printf("META,rss_kb_start,%" PRIu64 "\n",rss_kb());
    MM_Startup(); CHECK(CA_Startup(data));
    bench_cpu("empty", setup_empty_scene);
    bench_cpu("populated", setup_populated_scene);
    if(do_sdl) bench_sdl(data,user);
    CA_Shutdown(); MM_Shutdown();
    printf("META,rss_kb_end,%" PRIu64 "\n",rss_kb());
    printf("META,mm_total_bytes,%" PRIu64 "\n",(uint64_t)MM_TotalBytes());
    return 0;
}
