/* SPDX-License-Identifier: GPL-2.0-only */
#include "engine/WL_RENDER.h"
#include "engine/WL_SAVE.h"
#include "engine/WL_MAIN.h"
#include "engine/WL_DISPLAY.h"
#include "engine/ID_CA.h"
#include "engine/ID_VL.h"
#include "engine/ID_PM.h"
#include "engine/ID_US_1.h"
#include "platform/sdl_video.h"
#include "platform/sdl_audio.h"
#include "engine/ID_SD.h"
#include "platform/config_file.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%d: %s (%s; %s)\n",__LINE__,#x,CA_Error(),SDL_GetError());exit(1); } } while(0)
#include "finish_loading.h"
static uint32_t Hash(const void *data,size_t n) { const unsigned char *p=data;uint32_t h=2166136261U;while(n--) h=(h^*p++)*16777619U;return h; }
static void Pump(void) { CHECK(!VL_PollQuit());CHECK(VW_UpdateScreen()); }
/* Force long frames without stalling any individual rendering batch. The
   real dummy device drains concurrently, so servicing only once per frame
   would empty the 24-32 ms queue. */
static bool with_audio;
static unsigned services,empty;
static uint64_t last_service,max_gap;
static void ServiceAudio(void)
{
    SDL_Delay(1);
    uint64_t now=SDL_GetTicksNS(),gap=now-last_service;
    if(gap>max_gap) max_gap=gap;
    last_service=now;
    if(!Audio_TestQueuedBytes()) ++empty;
    CHECK(Audio_TestQueuedBytes()>=0);Audio_Pump();++services;
}
static void AudioStress(void)
{
    CHECK(Audio_Startup());SD_StartMusic(7);Audio_Pump();
    unsigned policy=Audio_TestTargetMS();CHECK(policy>=80 && policy<=250);
    Audio_TestQueueMS(24);Audio_Pump();SDL_Delay(60);int short_queue=Audio_TestQueuedBytes();
    Audio_TestQueueMS(policy);Audio_Pump();SDL_Delay(60);int safe_queue=Audio_TestQueuedBytes();
    CHECK(short_queue==0 && safe_queue>0);
    printf("Audio 60 ms stall: 24 ms policy %d bytes, %u ms policy %d bytes remaining.\n",short_queue,policy,safe_queue);
    Audio_Pump();
    int queued_ahead=Audio_TestQueuedBytes();CHECK(queued_ahead>0);
    CHECK(SD_PlaySound(ATKPISTOLSND,0,0));
    printf("Effect request: %.2f ms already queued before synthesis (device/OS latency additional).\n",(double)queued_ahead/192.0);
    uint8_t *pixels=malloc(1920*1080);int32_t depth[1920];wl_render_target t;
    CHECK(pixels && WL_TargetInit(&t,pixels,1920*1080,1920,1080,1920));
    CHECK(NewViewSize(19));last_service=SDL_GetTicksNS();uint64_t start=last_service;
    for(unsigned i=0;i<3;++i) CHECK(WL_RenderFrame(&t,depth,1920,true,ServiceAudio));
    CHECK(services>300 && SDL_GetTicksNS()-start>32000000);
    CHECK(!empty); /* No drained queue despite frames longer than its capacity. */
    sd_status status;SD_GetStatus(&status);CHECK(status.frames>48000/10);
    printf("Audio render stress: %u services, %u empty queues, longest gap %.2f ms.\n",services,empty,(double)max_gap/1000000);
    free(pixels);Audio_Shutdown();CHECK(NewViewSize(15));
}
/* Synthetic desktop modes exercise the production Auto replacement path;
   SDL's real output remains independently sized. No physical monitor claim. */
static void AutoScaling(const char *directory)
{
    display_settings d;CHECK(VL_DisplayGet(&d));
    d.high_resolution=true;d.smooth=false;d.render_auto=true;d.aspect=2;
    d.linear=d.integer=d.vsync=false;d.scale=0;d.width=3840;d.height=2160;
    VL_TestDesktopSize(3840,2160);CHECK(VL_DisplaySet(&d));Pump();
    uint64_t creations=VL_TestSurfaceCreations();
    SDL_Texture *texture=VL_TestSceneTexture();float w,h;
    CHECK(texture && SDL_GetTextureSize(texture,&w,&h) && w==1920 && h==1080);
    SDL_ScaleMode filter;CHECK(SDL_GetTextureScaleMode(texture,&filter) && filter==SDL_SCALEMODE_NEAREST);
    VL_TestDesktopSize(2560,1440);Pump();CHECK(VL_TestSurfaceCreations()==creations);
    CHECK(VL_TestSceneTexture()==texture); /* Same capped canvas, no allocation. */
    VL_TestDesktopSize(3440,1440);VL_TestDisplayFailure(5);Pump();
    CHECK(VL_TestSceneTexture()==texture && VL_TestSurfaceCreations()==creations);
    Pump();CHECK(VL_TestSceneTexture()==texture); /* Retry is throttled. */
    VL_TestDesktopSize(3440,1440);VL_TestDisplayFailure(6);Pump();
    CHECK(VL_TestSceneTexture()==texture && VL_TestSurfaceCreations()==creations);
    const unsigned modes[][4]={{3440,1440,1920,803},{3840,1080,1920,540},
        {1600,1200,1440,1080},{1280,720,1280,720},{3840,2160,1920,1080}};
    for(unsigned i=0;i<sizeof(modes)/sizeof(modes[0]);++i) {
        VL_TestDesktopSize(modes[i][0],modes[i][1]);Pump();
        CHECK(VL_TestSurfaceCreations()==++creations);
        CHECK(SDL_GetTextureSize(VL_TestSceneTexture(),&w,&h) && w==modes[i][2] && h==modes[i][3]);
        Pump();CHECK(VL_TestSurfaceCreations()==creations);
    }
    /* Exact RGBA oracle for the 1080p -> synthetic 4K nearest path. */
    char path[4096];CHECK(snprintf(path,sizeof(path),"%s/auto-4k.bmp",directory)>0);
    CHECK(VL_Capture(path));SDL_Surface *bmp=SDL_LoadBMP(path);CHECK(bmp);
    SDL_Surface *image=SDL_ConvertSurface(bmp,SDL_PIXELFORMAT_RGBA32);SDL_DestroySurface(bmp);
    CHECK(image);
    bool exact_4k=image->w==3840 && image->h==2160;
    CHECK(strcmp(SDL_GetCurrentVideoDriver(),"dummy") || exact_4k);
    if(!exact_4k) printf("Exact 4K pixel check unavailable: capture is %dx%d (window-manager limit).\n",image->w,image->h);
    unsigned char *pixels=malloc(1920*1080);int32_t depth[1920];wl_render_target t;
    CHECK(pixels && WL_TargetInit(&t,pixels,1920*1080,1920,1080,1920));
    CHECK(WL_RenderFrameAspect(&t,depth,1920,true,16,9,NULL));
    for(unsigned y=0;exact_4k && y<2160;++y) for(unsigned x=0;x<3840;++x) {
        unsigned index=pixels[(y/2)*1920+x/2];
        const unsigned char *actual=(const unsigned char *)image->pixels+(size_t)y*(unsigned)image->pitch+x*4;
        for(unsigned c=0;c<3;++c) {
            unsigned value=palette[index*3+c];CHECK(actual[c]==((value<<2)|(value>>4)));
        }
        CHECK(actual[3]==255);
    }
    free(pixels);SDL_DestroySurface(image);
    CHECK(VL_DisplayGet(&d));
    if(d.pending) { VL_TestExpireDisplayRequest();Pump();CHECK(VL_DisplayGet(&d)); }
    d.linear=true;CHECK(VL_DisplaySet(&d));Pump();
    CHECK(VL_TestSurfaceCreations()==creations);
    CHECK(SDL_GetTextureScaleMode(VL_TestSceneTexture(),&filter) && filter==SDL_SCALEMODE_LINEAR);
    VL_TestDesktopSize(0,0);d.render_auto=false;d.linear=false;d.width=960;d.height=720;
    CHECK(VL_DisplaySet(&d));Pump();
    VL_TestExpireDisplayRequest();Pump();
}
static void Wide(void)
{
    CHECK(NewViewSize(19));CHECK(GameDrawPaused());
    uint8_t *narrow=malloc(640*480),*wide=malloc(1280*480);int32_t depth[4096];wl_render_target a,b;
    CHECK(narrow && wide && WL_TargetInit(&a,narrow,640*480,640,480,640) && WL_TargetInit(&b,wide,1280*480,1280,480,1280));
    CHECK(WL_RenderFrameAspect(&a,depth,4096,true,4,3,NULL));
    CHECK(WL_RenderFrameAspect(&b,depth,4096,true,8,3,NULL));
    /* Same vertical framing and exact central pixels; extra columns are rays,
       not horizontally stretched pixels. HUD is centered independently. */
    unsigned vx=(unsigned)VIEWX*2,vw=(unsigned)VIEWWIDTH*2,vy=(unsigned)VIEWY*480/200,vh=(unsigned)VIEWHEIGHT*480/200;
    for(unsigned y=vy;y<vy+vh;++y) CHECK(!memcmp(narrow+y*640+vx,wide+y*1280+320+vx,vw));
    for(unsigned y=384;y<480;++y) CHECK(!memcmp(narrow+y*640,wide+y*1280+320,640));
    uint32_t before=Hash(framebuffer,VL_PIXELS);unsigned rng=US_RndIndex();
    const unsigned ratios[][2]={{4,3},{16,9},{16,10},{21,9},{32,9}};
    const unsigned sizes[][2]={{640,480},{1280,720},{1920,1080},{3440,1440},{3840,1080},{4096,2160}};
    for(unsigned n=0;n<6;++n) {
        size_t count=(size_t)sizes[n][0]*sizes[n][1];uint8_t *p=malloc(count+16);CHECK(p);memset(p+count,0xad,16);
        wl_render_target t;CHECK(WL_TargetInit(&t,p,count,sizes[n][0],sizes[n][1],sizes[n][0]));
        uint64_t begin=SDL_GetTicksNS();
        for(unsigned r=0;r<5;++r) {
            for(unsigned v=4;v<=19;v+=15) { CHECK(NewViewSize(v));CHECK(GameDrawPaused());
                CHECK(WL_RenderFrameAspect(&t,depth,4096,true,ratios[r][0],ratios[r][1],NULL));
                wl_rectangle scene,ui;CHECK(WL_AspectFit(t.width,t.height,ratios[r][0],ratios[r][1],&scene));
                CHECK(WL_AspectFit(scene.width,scene.height,4,3,&ui));ui.x+=scene.x;ui.y+=scene.y;
                for(unsigned y=ui.height*4/5;y<ui.height;++y) for(unsigned x=0;x<scene.width;++x) {
                    unsigned tx=scene.x+x;
                    uint8_t expected=tx<ui.x || tx>=ui.x+ui.width?127:framebuffer[(size_t)(y*200/ui.height)*320+(tx-ui.x)*320/ui.width];
                    CHECK(t.pixels[(size_t)(ui.y+y)*t.stride+tx]==expected);
                }
            }
        }
        printf("Render benchmark %ux%u: %.2f ms/frame (ten aspect/view combinations), %zu indexed bytes.\n",sizes[n][0],sizes[n][1],(double)(SDL_GetTicksNS()-begin)/10000000,count);
        for(unsigned i=0;i<16;++i) CHECK(p[count+i]==0xad);
        free(p);
    }
    CHECK(US_RndIndex()==rng);CHECK(NewViewSize(19));CHECK(GameDrawPaused());CHECK(Hash(framebuffer,VL_PIXELS)==before);
    display_settings d;CHECK(VL_DisplayGet(&d));d.high_resolution=true;d.render_auto=true;d.aspect=5;d.linear=true;
    CHECK(VL_DisplaySet(&d));Pump();CHECK(VL_DisplayGet(&d) && d.render_auto && d.aspect==5 && d.linear);
    CHECK(Config_Save(SDL_getenv("XDG_DATA_HOME")));Audio_Shutdown();VL_Shutdown();CHECK(VL_Startup());if(with_audio) CHECK(Audio_Startup());CHECK(Config_Load(SDL_getenv("XDG_DATA_HOME"))==CONFIG_LOADED);Pump();
    CHECK(VL_DisplayGet(&d) && d.render_auto && d.aspect==5 && d.linear);
    /* VSync may be unsupported by a backend. A rejected request must leave
       the preference and all other settings unchanged. */
    d.vsync=true;bool accepted=VL_DisplaySet(&d);CHECK(VL_DisplayGet(&d));CHECK(d.vsync==accepted);
    d.vsync=!d.vsync;VL_TestDisplayFailure(7);CHECK(!VL_DisplaySet(&d));CHECK(VL_DisplayGet(&d) && d.vsync==accepted);
    d.linear=!d.linear;VL_TestDisplayFailure(8);CHECK(!VL_DisplaySet(&d));CHECK(VL_DisplayGet(&d) && d.linear);
    /* Auto dimensions are staged before committing a display change too. */
    d.render_auto=false;d.render_width=640;d.render_height=480;d.vsync=false;CHECK(VL_DisplaySet(&d));Pump();
    d.render_auto=true;VL_TestDisplayFailure(5);CHECK(!VL_DisplaySet(&d));CHECK(VL_DisplayGet(&d) && !d.render_auto);Pump();
    CHECK(VL_Screenshot());
    SDL_Event key;SDL_zero(key);key.type=SDL_EVENT_WINDOW_FOCUS_GAINED;CHECK(SDL_PushEvent(&key));Pump();
    SDL_zero(key);key.type=SDL_EVENT_KEY_DOWN;key.key.scancode=SDL_SCANCODE_F12;CHECK(SDL_PushEvent(&key));Pump();
    key.key.repeat=true;CHECK(SDL_PushEvent(&key));Pump();key.type=SDL_EVENT_KEY_UP;CHECK(SDL_PushEvent(&key));Pump();
    CHECK(US_RndIndex()==rng);free(narrow);free(wide);CHECK(NewViewSize(15));CHECK(GameDrawPaused());
}
static void DeathFrame(wl_render_target *t,unsigned aw,unsigned ah,unsigned expected)
{
    int32_t depth[1920];uint32_t seed,before=Hash(framebuffer,VL_PIXELS),clock=gamestate.TimeCount;
    unsigned phase=GameDeathPresentation(&seed),rng=US_RndIndex();int angle=player.angle;
    CHECK(phase==expected && WL_RenderFrameAspect(t,depth,1920,true,aw,ah,NULL));
    CHECK(before==Hash(framebuffer,VL_PIXELS) && clock==gamestate.TimeCount && rng==US_RndIndex() && angle==player.angle);
    uint32_t after;CHECK(GameDeathPresentation(&after)==phase && after==seed);
    wl_rectangle r;CHECK(WL_AspectFit(t->width,t->height,aw,ah,&r));
    unsigned vx=r.x+(unsigned)VIEWX*r.width/320,vy=r.y+(unsigned)VIEWY*r.height/200;
    unsigned right=r.x+((unsigned)VIEWX+VIEWWIDTH)*r.width/320,bottom=r.y+((unsigned)VIEWY+VIEWHEIGHT)*r.height/200;
    /* This lies outside the former 4:3 world rectangle in widescreen. The
       scene must remain geometry (or fizzle red), never the old teal border. */
    CHECK(t->pixels[(size_t)(vy+1)*t->stride+vx+1]!=127);
    if(phase==3) for(unsigned y=vy;y<bottom;++y) for(unsigned x=vx;x<right;++x) CHECK(t->pixels[(size_t)y*t->stride+x]==4);
    Pump();
    int n;SDL_Window **windows=SDL_GetWindows(&n);CHECK(windows && n==1);
    SDL_FRect rect;CHECK(SDL_GetRenderLogicalPresentationRect(SDL_GetRenderer(windows[0]),&rect));SDL_free(windows);
    CHECK(fabs((double)rect.w*ah-(double)rect.h*aw)<(double)(aw+ah));
}
static void DeathContinuity(void)
{
    static const unsigned sizes[][2]={{640,480},{1280,720},{1920,1080}};
    static const unsigned ratios[][2]={{4,3},{16,9},{16,10},{21,9},{32,9},{16,9}};
    uint8_t *pixels=malloc(1920*1080);CHECK(pixels);unsigned duration=0;
    for(unsigned n=0;n<3;++n) for(unsigned a=0;a<6;++a) {
        CHECK(NewViewSize(15) && GameStart(0));player.angle=0;CHECK(GameDrawPaused());
        display_settings d;CHECK(VL_DisplayGet(&d));d.high_resolution=true;d.render_auto=d.integer=false;
        d.render_width=sizes[n][0];d.render_height=sizes[n][1];d.aspect=a==5?0:a+1;d.scale=0;d.width=1280;d.height=720;CHECK(VL_DisplaySet(&d));Pump();
        wl_render_target t;CHECK(WL_TargetInit(&t,pixels,1920*1080,sizes[n][0],sizes[n][1],sizes[n][0]));
        objtype attacker=player;attacker.y-=TILEGLOBAL;TakeDamage(1000,&attacker);CHECK(gamestate.died);
        uint64_t tic=1;CHECK(GameStep(tic));DeathFrame(&t,ratios[a][0],ratios[a][1],1);
        while(GameDeathPresentation(NULL)<2 && tic<100) CHECK(GameStep(++tic));
        DeathFrame(&t,ratios[a][0],ratios[a][1],2);
        for(unsigned i=0;i<10;++i) CHECK(GameStep(++tic));
        DeathFrame(&t,ratios[a][0],ratios[a][1],2);
        /* A red Classic source can stay byte-identical while the native
           fizzle cursor advances. It still needs a new native upload. */
        for(unsigned y=0;y<VIEWHEIGHT;++y) memset(framebuffer+((unsigned)VIEWY+y)*320+VIEWX,4,VIEWWIDTH);
        Pump();uint64_t uploads=VL_TestSceneUploads();CHECK(GameStep(++tic));Pump();
        CHECK(VL_TestSceneUploads()==uploads+1);
        while(GameDeathPresentation(NULL)<3 && tic<300) CHECK(GameStep(++tic));
        DeathFrame(&t,ratios[a][0],ratios[a][1],3);
        while(gamestate.died && tic<600) CHECK(GameStep(++tic));
        CHECK(!gamestate.died && gamestate.lives==2);
        DeathFrame(&t,ratios[a][0],ratios[a][1],0);
        if(!duration) duration=(unsigned)tic;
        CHECK(duration==tic);
    }
    free(pixels);printf("Death rotation, fizzle, red hold and respawn keep every aspect at all presets (%u tics).\n",duration);
}
static void OwnedLevels(void)
{
    uint8_t *pixels=malloc(640*480);wl_render_target t;int32_t depth[640];
    CHECK(pixels && WL_TargetInit(&t,pixels,640*480,640,480,640));
    for(unsigned map=0;map<60;++map) {
        CHECK(GameStartMap(0,map,2,0));
        for(int angle=0;angle<360;angle+=90) {
            player.angle=angle;CHECK(GameDrawPaused());
            unsigned rng=US_RndIndex();uint32_t before=Hash(framebuffer,VL_PIXELS);
            CHECK(WL_RenderFrameAspect(&t,depth,640,true,16,9,NULL));
            CHECK(rng==US_RndIndex() && before==Hash(framebuffer,VL_PIXELS));
        }
    }
    free(pixels);puts("All 60 owned maps rendered natively in four cardinal directions.");
}
static void Visual(void)
{
    CHECK(GameStart(0));InitObjects();CHECK(NewViewSize(15));
    player.x=2*TILEGLOBAL+32768;player.y=2*TILEGLOBAL+32768;player.angle=0;
    uint8_t *pixels=malloc(640*480);CHECK(pixels);int32_t depth[640];wl_render_target t;
    CHECK(WL_TargetInit(&t,pixels,640*480,640,480,640));
    unsigned char *weapon=(unsigned char *)PM_GetPage(PMSpriteStart+SPR_PISTOLREADY,NULL);
    unsigned char original[14];memcpy(original,weapon,14);weapon[6]=weapon[7]=0;
    unsigned char *near=(unsigned char *)PM_GetPage(PMSpriteStart+SPR_STAT_0,NULL);
    unsigned char *far=(unsigned char *)PM_GetPage(PMSpriteStart+SPR_STAT_1,NULL);
    memset(near+14,10,32);memset(far+14,20,32);
    CHECK(WL_RenderFrame(&t,depth,640,true,NULL));uint8_t background=pixels[193*640+319];
    statcount=2;statobjlist[0]=(statobj_t){4,2,SPR_STAT_0,dressing,false};
    statobjlist[1]=(statobj_t){5,2,SPR_STAT_1,dressing,false};
    CHECK(WL_RenderFrame(&t,depth,640,true,NULL));CHECK(pixels[193*640+319]==10);
    /* A transparent neighbor remains background, while index zero is opaque. */
    CHECK(pixels[193*640+300]!=10 && pixels[193*640+300]!=20);
    statobj_t tmp=statobjlist[0];statobjlist[0]=statobjlist[1];statobjlist[1]=tmp;
    CHECK(WL_RenderFrame(&t,depth,640,true,NULL));CHECK(pixels[193*640+319]==10);
    memset(near+14,0,32);CHECK(WL_RenderFrame(&t,depth,640,true,NULL));CHECK(!pixels[193*640+319]);
    /* Wall occlusion is independent of actor-list order and screen scaling. */
    for(unsigned y=1;y<63;++y) tilemap[3][y]=1;
    CHECK(WL_RenderFrame(&t,depth,640,true,NULL));CHECK(pixels[193*640+319]!=0 && pixels[193*640+319]!=20);
    memcpy(weapon,original,14);statcount=0;CHECK(WL_RenderFrame(&t,depth,640,true,NULL));
    CHECK(pixels[300*640+317]>=100); /* original weapon art stays in the lower center */
    CHECK(background!=10 && background!=20);
    free(pixels);GameStop();
}
extern statetype s_grdstand;
static void BaselineWrite(const char *directory,const char *name,const uint8_t *pixels,
                          unsigned width,unsigned height,unsigned stride)
{
    char path[4096];CHECK(snprintf(path,sizeof(path),"%s/%s.w3i",directory,name)>0);
    FILE *file=fopen(path,"wb");CHECK(file);
    const uint8_t header[16]={'W','3','R','I',1,0,0,0,
        (uint8_t)width,(uint8_t)(width>>8),(uint8_t)(width>>16),(uint8_t)(width>>24),
        (uint8_t)height,(uint8_t)(height>>8),(uint8_t)(height>>16),(uint8_t)(height>>24)};
    CHECK(fwrite(header,1,sizeof(header),file)==sizeof(header));
    for(unsigned y=0;y<height;++y)
        CHECK(fwrite(pixels+(size_t)y*stride,1,width,file)==width);
    CHECK(!fclose(file));
}
static void BaselineClassicOutput(const char *directory,const char *name,unsigned width,unsigned height)
{
    size_t count=(size_t)width*height;uint8_t *pixels=calloc(count,1);CHECK(pixels);
    uint64_t lhs=(uint64_t)width*3,rhs=(uint64_t)height*4;
    unsigned cw=lhs<=rhs?width:(unsigned)((uint64_t)height*4/3);
    unsigned ch=lhs<=rhs?(unsigned)((uint64_t)width*3/4):height;
    unsigned ox=(width-cw)/2,oy=(height-ch)/2;
    for(unsigned y=0;y<ch;++y) for(unsigned x=0;x<cw;++x)
        pixels[(size_t)(oy+y)*width+ox+x]=framebuffer[(size_t)(y*200/ch)*320+x*320/cw];
    BaselineWrite(directory,name,pixels,width,height,width);free(pixels);
}
static void BaselineEnhanced(const char *directory,const char *name,unsigned width,unsigned height,
                             unsigned aspect_width,unsigned aspect_height)
{
    size_t count=(size_t)width*height;uint8_t *pixels=malloc(count);int32_t *depth=malloc((size_t)width*sizeof(*depth));
    wl_render_target target;CHECK(pixels && depth && WL_TargetInit(&target,pixels,count,width,height,width));
    CHECK(WL_RenderFrameAspect(&target,depth,width,true,aspect_width,aspect_height,NULL));
    BaselineWrite(directory,name,pixels,width,height,width);free(depth);free(pixels);
}
static void BaselineScene(void)
{
    CHECK(GameStart(0));InitObjects();CHECK(NewViewSize(15));
    memset(tilemap,0,sizeof(tilemap));memset(areaMap,0,sizeof(areaMap));
    for(unsigned i=0;i<64;++i) tilemap[0][i]=tilemap[63][i]=tilemap[i][0]=tilemap[i][63]=1;
    for(unsigned x=1;x<=12;++x) tilemap[x][1]=tilemap[x][5]=1;
    for(unsigned y=1;y<=5;++y) tilemap[12][y]=1;
    player.x=2*TILEGLOBAL+32768;player.y=3*TILEGLOBAL+32768;player.tilex=2;player.tiley=3;player.angle=0;
    gamestate.weapon=gamestate.chosenweapon=gamestate.bestweapon=wp_pistol;gamestate.weaponframe=0;
    statcount=2;statobjlist[0]=(statobj_t){4,2,SPR_STAT_0,dressing,false};
    statobjlist[1]=(statobj_t){7,4,SPR_STAT_1,dressing,false};
    objtype *actor=SpawnActor(6,3,&s_grdstand);CHECK(actor);actor->angle=180;actor->dir=west;actor->active=true;
    doornum=1;tilemap[9][3]=128;doorobjlist[0]=(doorobj_t){9,3,0,true,dr_closed,0,0};
    ResetPushwalls();
}
static void BaselinePair(const char *directory,const char *stem)
{
    char name[128];CHECK(GameDrawPaused());
    CHECK(snprintf(name,sizeof(name),"%s-classic-320x200",stem)>0);BaselineWrite(directory,name,framebuffer,320,200,320);
    CHECK(snprintf(name,sizeof(name),"%s-enhanced-1280x720-16x9",stem)>0);BaselineEnhanced(directory,name,1280,720,16,9);
}
static void Baselines(const char *directory)
{
    char path[4096];CHECK(snprintf(path,sizeof(path),"%s/palette.rgb6",directory)>0);
    FILE *file=fopen(path,"wb");CHECK(file && fwrite(palette,1,768,file)==768 && !fclose(file));

    BaselineScene();BaselinePair(directory,"corridor-static");
    BaselineClassicOutput(directory,"corridor-static-classic-output-640x480",640,480);
    BaselineClassicOutput(directory,"corridor-static-classic-output-1280x720",1280,720);
    BaselineClassicOutput(directory,"corridor-static-classic-output-1001x601",1001,601);
    BaselineEnhanced(directory,"corridor-static-enhanced-640x480-4x3",640,480,4,3);
    BaselineEnhanced(directory,"corridor-static-enhanced-1280x800-16x10",1280,800,16,10);
    BaselineEnhanced(directory,"corridor-static-enhanced-1680x720-21x9",1680,720,21,9);
    BaselineEnhanced(directory,"corridor-static-enhanced-2560x720-32x9",2560,720,32,9);
    BaselineEnhanced(directory,"corridor-static-enhanced-1001x601-auto",1001,601,1001,601);
    GameStop();

    static const uint16_t door_positions[]={0,32768,65535};
    static const char *door_names[]={"closed","half-open","open"};
    for(unsigned i=0;i<3;++i) { BaselineScene();doorobjlist[0].position=door_positions[i];doorobjlist[0].action=i==2?dr_open:dr_opening;
        char stem[64];CHECK(snprintf(stem,sizeof(stem),"door-motion-%s",door_names[i])>0);BaselinePair(directory,stem);GameStop(); }

    static const unsigned wall_states[]={0,32,0};
    static const char *wall_names[]={"initial","moving","tile-boundary"};
    for(unsigned i=0;i<3;++i) { BaselineScene();doornum=0;tilemap[9][3]=0;
        pwallstate=i==2?128:i==1?64:1;pwallpos=wall_states[i];pwallx=i==2?9:8;pwally=3;pwalldir=east;
        tilemap[8][3]=i==2?0:0xc1;tilemap[9][3]=i==2?0xc1:1;tilemap[10][3]=i==2?1:0;
        char stem[64];CHECK(snprintf(stem,sizeof(stem),"pushwall-motion-%s",wall_names[i])>0);BaselinePair(directory,stem);GameStop(); }

    BaselineScene();statobjlist[0].tilex=4;statobjlist[0].tiley=1;statobjlist[1].tilex=4;statobjlist[1].tiley=5;
    BaselinePair(directory,"edge-clipping");GameStop();

    BaselineScene();CHECK(GameDrawPaused());
    BaselineClassicOutput(directory,"display-transition-classic-initial-640x480",640,480);
    BaselineClassicOutput(directory,"display-transition-classic-letterbox-1280x720",1280,720);
    BaselineClassicOutput(directory,"display-transition-classic-resize-1001x601",1001,601);
    BaselineClassicOutput(directory,"display-transition-classic-restored-640x480",640,480);
    BaselineEnhanced(directory,"display-transition-enhanced-initial-640x480-4x3",640,480,4,3);
    BaselineEnhanced(directory,"display-transition-enhanced-wide-1280x720-16x9",1280,720,16,9);
    BaselineEnhanced(directory,"display-transition-enhanced-auto-1001x601",1001,601,1001,601);
    BaselineEnhanced(directory,"display-transition-enhanced-restored-640x480-4x3",640,480,4,3);
    GameStop();puts("Synthetic Classic/Enhanced visual baselines captured.");
}
int main(int argc,char **argv)
{
    CHECK(argc==4);
    const float rates[]={60,75,90,120,144,165,180,240,73,59.94f};
    for(unsigned i=0;i<sizeof(rates)/sizeof(rates[0]);++i)
        CHECK(fabs(VL_TestRefreshRate(rates[i],60)-rates[i])<0.00001);
    CHECK(VL_TestRefreshRate(0,75)==75 && VL_TestRefreshRate(NAN,90)==90);
    CHECK(VL_TestRefreshRate(INFINITY,0)==60 && VL_TestRefreshRate(-1,NAN)==60);
    CHECK(VL_TestRefreshRate(360,60)==240 && VL_TestRefreshRate(0,500)==240);
    CHECK(VL_MonitorRefreshRate()==60);
    MM_Startup();CHECK(CA_Startup(argv[1]));CHECK(VL_SetPalette(gamepal,768));CHECK(VL_Startup());
    with_audio=!strcmp(argv[3],"owned");if(with_audio) CHECK(Audio_Startup());
    Display_Connect(VL_DisplayGet,VL_DisplaySet);CHECK(Save_SetDirectory(argv[2]));CHECK(DemoLoopStartup(0,true));CHECK(DemoLoopStartGame(0));
    TestFinishLoading(0);CHECK(GameStep(14));VL_FadeStep(20);CHECK(Save_Write(0,"render before",true));
    unsigned rng=US_RndIndex();uint32_t frame=Hash(framebuffer,VL_PIXELS),clock=gamestate.TimeCount;
    objtype saved_player=player,saved_actors[MAXACTORS];memcpy(saved_actors,objlist,sizeof(saved_actors));
    bool saved_vis[64][64];memcpy(saved_vis,spotvis,sizeof(saved_vis));
    static const unsigned sizes[][2]={{640,480},{1280,720},{1920,1080}};
    for(unsigned n=0;n<3;++n) {
        unsigned w=sizes[n][0],h=sizes[n][1],stride=w+8;size_t count=(size_t)stride*h;
        uint8_t *pixels=malloc(count+32);int32_t depth[1920];CHECK(pixels);memset(pixels,0xac,count+32);
        wl_render_target target;CHECK(WL_TargetInit(&target,pixels,count,w,h,stride));
        CHECK(!WL_RenderFrame(&target,depth,w-1,true,NULL));
        for(unsigned view=4;view<=19;++view) {
            CHECK(NewViewSize(view));CHECK(GameDrawPaused());
            uint32_t before=Hash(framebuffer,VL_PIXELS);
            CHECK(WL_RenderFrame(&target,depth,1920,true,NULL));
            CHECK(Hash(framebuffer,VL_PIXELS)==before && US_RndIndex()==rng && gamestate.TimeCount==clock);
            for(unsigned y=0;y<h;++y) for(unsigned x=w;x<stride;++x) CHECK(pixels[(size_t)y*stride+x]==0xac);
            for(unsigned i=0;i<32;++i) CHECK(pixels[count+i]==0xac);
            unsigned left,cw;WL_RenderContent(w,h,&left,&cw);
            CHECK(cw*3==h*4);
            /* Original HUD must be exactly aspect-scaled, including index zero. */
            for(unsigned y=h*4/5;y<h;++y) for(unsigned x=0;x<cw;++x)
                CHECK(pixels[(size_t)y*stride+left+x]==framebuffer[(size_t)(y*200/h)*320+x*320/cw]);
            for(unsigned y=0;y<h;++y) for(unsigned x=0;x<left;++x) CHECK(!pixels[(size_t)y*stride+x] && !pixels[(size_t)y*stride+w-1-x]);
            /* A second pure render produces the same image, with no allocation. */
            uint32_t image=Hash(pixels,count);CHECK(WL_RenderFrame(&target,depth,1920,true,NULL));CHECK(Hash(pixels,count)==image);
        }
        CHECK(NewViewSize(15));CHECK(GameDrawPaused());
        CHECK(WL_RenderFrame(&target,depth,1920,true,NULL));
        uint32_t native=Hash(pixels,count);CHECK(WL_RenderFrame(&target,depth,1920,false,NULL));
        CHECK(Hash(pixels,count)!=native); /* Genuine geometry differs from enlarged DOS pixels. */
        free(pixels);
        display_settings d;CHECK(VL_DisplayGet(&d));d.high_resolution=true;d.aspect=1;d.render_width=w;d.render_height=h;CHECK(VL_DisplaySet(&d));Pump();
        CHECK(VL_DisplayGet(&d) && d.high_resolution && d.render_width==w && d.render_height==h);
        uint64_t uploads=VL_TextureUploads(),scenes=VL_TestSceneUploads();Pump();CHECK(VL_TextureUploads()==uploads && VL_TestSceneUploads()==scenes);
        CHECK(GameDrawPaused());Pump();CHECK(VL_TextureUploads()==uploads && VL_TestSceneUploads()==scenes+1);
        CHECK(Config_Save(argv[2]));d.high_resolution=false;CHECK(VL_DisplaySet(&d));Pump();
        CHECK(Config_Load(argv[2])==CONFIG_LOADED);Pump();CHECK(VL_DisplayGet(&d) && d.high_resolution && d.render_width==w);
        /* Recreate SDL completely to verify actual launch-time restoration. */
        Audio_Shutdown();VL_Shutdown();CHECK(VL_Startup());if(with_audio) CHECK(Audio_Startup());CHECK(Config_Load(argv[2])==CONFIG_LOADED);Pump();
        CHECK(VL_DisplayGet(&d) && d.high_resolution && d.render_width==w);
        int count_windows;SDL_Window **windows=SDL_GetWindows(&count_windows);CHECK(windows && count_windows==1);
        SDL_Window *window=windows[0];SDL_free(windows);
        const int windowsizes[][2]={{640,480},{1920,1080},{600,1000},{160,120},{1,1},{960,720}};
        for(unsigned i=0;i<6;++i) {
            CHECK(SDL_SetWindowSize(window,windowsizes[i][0],windowsizes[i][1]) && SDL_SyncWindow(window));Pump();
            int pw,ph;CHECK(SDL_GetWindowSizeInPixels(window,&pw,&ph));CHECK(VL_DisplayGet(&d));
            CHECK(d.pixel_width==pw && d.pixel_height==ph);
            if(pw>=4 && ph>=3) { SDL_FRect rect;CHECK(SDL_GetRenderLogicalPresentationRect(SDL_GetRenderer(window),&rect));CHECK(fabs(rect.w*3-rect.h*4)<=4); }
        }
        d.fullscreen=true;CHECK(VL_DisplaySet(&d) && SDL_SyncWindow(window));Pump();
        CHECK(VL_DisplayGet(&d) && d.fullscreen);d.fullscreen=false;CHECK(VL_DisplaySet(&d) && SDL_SyncWindow(window));Pump();
    }
    CHECK(!memcmp(&player,&saved_player,sizeof(player)) && !memcmp(saved_actors,objlist,sizeof(saved_actors)));
    CHECK(!memcmp(saved_vis,spotvis,sizeof(saved_vis)) && Hash(framebuffer,VL_PIXELS)==frame);
    CHECK(Save_Write(1,"render after",true));
    /* Failed allocations/resolutions/fullscreen requests retain the old surface. */
    display_settings d,old;CHECK(VL_DisplayGet(&old));d=old;d.render_width=1234;CHECK(!VL_DisplaySet(&d));
    d=old;d.render_width=640;d.render_height=480;VL_TestDisplayFailure(5);CHECK(!VL_DisplaySet(&d));Pump();
    CHECK(VL_DisplayGet(&d) && d.render_width==old.render_width && d.high_resolution);
    d=old;d.render_width=640;d.render_height=480;VL_TestDisplayFailure(6);CHECK(!VL_DisplaySet(&d));Pump();
    d=old;d.high_resolution=false;d.fullscreen=true;VL_TestDisplayFailure(1);CHECK(!VL_DisplaySet(&d));Pump();
    CHECK(VL_DisplayGet(&d) && d.high_resolution);
    /* Switching display preference cannot consume a simulation tic or RNG. */
    for(unsigned n=0;n<30;++n) { d.high_resolution=!d.high_resolution;CHECK(VL_DisplaySet(&d));Pump();CHECK(US_RndIndex()==rng && gamestate.TimeCount==clock); }
    Wide();AutoScaling(argv[2]);DeathContinuity();
    if(!strcmp(argv[3],"synthetic")) { Visual();const char *baseline=getenv("WOLF3D_BASELINE_DIR");if(baseline) Baselines(baseline); }
    else { AudioStress();OwnedLevels(); }
    GameStop();Audio_Shutdown();Display_Connect(NULL,NULL);VL_Shutdown();CA_Shutdown();MM_Shutdown();CHECK(!MM_TotalBytes());
    puts("Native render targets, all sizes/views, HUD, isolation, persistence, switching and failure recovery passed.");return 0;
}
