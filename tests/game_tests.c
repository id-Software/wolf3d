/* SPDX-License-Identifier: GPL-2.0-only */
#include "engine/WL_DEF.h"
#include "engine/WL_SAVE.h"
#include "engine/ID_CA.h"
#include "engine/ID_PM.h"
#include "engine/ID_IN.h"
#include "engine/ID_VL.h"
#include "engine/WL_MENU.h"
#include "engine/WL_MAIN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s (%s)\n",__LINE__,#x,CA_Error()); exit(1); } } while (0)
#include "finish_loading.h"
static void TestExpand(void)
{
    uint16_t out[12]; memset(out,0xcc,sizeof(out));
    const unsigned char near[]={0x34,0x12,3,0xa7,1};
    CHECK(CAL_CarmackExpand(near,sizeof(near),out,4));
    for(unsigned i=0;i<4;++i) CHECK(out[i]==0x1234);
    CHECK(out[4]==0xcccc);
    const unsigned char far[]={1,0,2,0,4,0xa8,0,0};
    CHECK(CAL_CarmackExpand(far,sizeof(far),out,6));
    for(unsigned i=0;i<6;++i) CHECK(out[i]==(i%2 ? 2 : 1));
    const unsigned char escape[]={0,0xa7,0x34,0,0xa8,0x56};
    CHECK(CAL_CarmackExpand(escape,sizeof(escape),out,2)); CHECK(out[0]==0xa734 && out[1]==0xa856);
    const unsigned char invalid[]={1,0xa7,0};
    CHECK(!CAL_CarmackExpand(invalid,sizeof(invalid),out,1));
    CHECK(!CAL_CarmackExpand(far,6,out,6));
    CHECK(!CAL_CarmackExpand(escape,2,out,1));
    CHECK(!CAL_CarmackExpand(near,sizeof(near),out,3));
    for(size_t n=0;n<sizeof(near);++n) CHECK(!CAL_CarmackExpand(near,n,out,4));
    const uint16_t rlew[]={1,0xabcd,4,2,3};
    CHECK(CA_RLEWexpand(rlew,5,out,6,0xabcd));
    CHECK(out[0]==1 && out[1]==2 && out[4]==2 && out[5]==3);
    CHECK(!CA_RLEWexpand(rlew,3,out,6,0xabcd));
    CHECK(!CA_RLEWexpand(rlew,5,out,3,0xabcd));
    const uint16_t zero[]={0xabcd,0,3}; CHECK(!CA_RLEWexpand(zero,3,out,1,0xabcd));
    uint32_t seed=7;
    for(unsigned k=0;k<2000;++k) {
        unsigned char bytes[32];uint16_t guarded[18];memset(guarded,0x57,sizeof(guarded));
        for(unsigned j=0;j<32;++j) { seed=seed*1664525U+1013904223U; bytes[j]=(unsigned char)(seed>>24); }
        (void)CAL_CarmackExpand(bytes,32,guarded+1,16);
        CHECK(guarded[0]==0x5757 && guarded[17]==0x5757);
    }
}
static void TestFixed(void)
{
    CHECK(FixedByFrac(65536,32768)==32768);
    CHECK(FixedByFrac(-65536,32768)==-32768);
    CHECK(FixedByFrac(-65536,0x80008000U)==32768);
    CHECK(FixedByFrac(65536,0x00010000U)==0); /* Cardinal high word is NOT 16.16 unity. */
    CHECK(FixedByFrac(INT32_MIN,0x8000ffffU)==2147450880);
    CHECK(FixedByFrac(INT32_MAX,0x0000ffffU)==2147450879);
    CHECK(WL_FloorShift(-1,16)==-1 && WL_FloorShift(-65537,16)==-2);
    CHECK(WL_FloorShift(INT64_MIN,63)==-1 && WL_FloorShift(INT64_MIN,16)==INT32_MIN);
}
static void Key(unsigned scan) { IN_KeyEvent(scan,true);IN_KeyEvent(scan,false); }
static void TestInputMenu(void)
{
    CHECK(SetupControlPanel(0)); MainItems.curpos=viewscores;
    Key(0x30); Key(0x2f); /* B then V between polls: both consumed in order. */
    CHECK(HandleMenu(0)==-2 && MainItems.curpos==viewscores);
    MainItems.curpos=quit; CHECK(HandleMenu(1)==-2 && MainItems.curpos==quit); /* No stale shortcut. */
    Key(0x30); Key(sc_Enter); CHECK(HandleMenu(2)==backtodemo);
    CHECK(HandleMenu(3)==-2); /* Enter consumed once, not held by its queued text. */
    Key(sc_Escape); Key(sc_Enter); CHECK(HandleMenu(4)==-1);
    CHECK(HandleMenu(5)==-2); /* All confirmation edges consumed atomically. */
    IN_ClearKeysDown();
}
static void TestScale(void)
{
    unsigned char column[64]; for(unsigned i=0;i<64;++i) column[i]=(unsigned char)i;
    memset(framebuffer,255,VL_PIXELS); ScalePost(0,1,256,column); /* 64-pixel wall. */
    for(unsigned y=0;y<64;++y) CHECK(framebuffer[(VIEWY+28+y)*320+VIEWX]==y);
    CHECK(framebuffer[(VIEWY+27)*320+VIEWX]==255);
    CHECK(framebuffer[(VIEWY+92)*320+VIEWX]==255);
}
static void TestWorld(bool synthetic,const char *dump)
{
    CHECK(SetupGameLevel());
    CHECK(CA_Map() && CA_Map()->number==0);
    if(synthetic) {
        CHECK(player.tilex==2 && player.tiley==2 && player.angle==0);
        for(unsigned i=0;i<4096;++i) CHECK(CA_Map()->planes[2][i]==7);
    } else CHECK(player.tilex==29 && player.tiley==57 && player.angle==0);
    size_t size;const unsigned char *page=PM_GetPage(1,&size);CHECK(page && size==4096);
    if(dump) {
        FILE *out=fopen(dump,"wb");CHECK(out);
        for(unsigned plane=0;plane<3;++plane) for(unsigned i=0;i<4096;++i) {
            unsigned value=CA_Map()->planes[plane][i];
            unsigned char bytes[2]={(unsigned char)value,(unsigned char)(value>>8)};
            CHECK(fwrite(bytes,1,2,out)==2);
        }
        CHECK(fwrite(page,1,4096,out)==4096);CHECK(fclose(out)==0);
    }
    CHECK(PM_GetPage(1,NULL)==page);
    if(synthetic) {
        for(unsigned x=0;x<64;++x) for(unsigned y=0;y<64;++y) CHECK(page[x*64+y]==(x*3+y+17)%256);
        unsigned char pixels[4096],mask[4096]; CHECK(PM_DecodeSprite(0,pixels,mask));
        CHECK(!mask[31*64+31] && mask[32*64+31] && pixels[32*64+31]==100);
        CHECK(pixels[63*64+31]==131 && !mask[32*64+30]);
        unsigned char *sprite=(unsigned char *)PM_GetPage(PMSpriteStart,NULL);
        unsigned char saved[46];memcpy(saved,sprite,46);
        sprite[0]=64;CHECK(!PM_DecodeSprite(0,pixels,mask));memcpy(sprite,saved,46);
        sprite[4]=254;sprite[5]=255;CHECK(!PM_DecodeSprite(0,pixels,mask));memcpy(sprite,saved,46);
        sprite[6]=129;CHECK(!PM_DecodeSprite(0,pixels,mask));memcpy(sprite,saved,46);
        sprite[8]=0;sprite[9]=128;CHECK(!PM_DecodeSprite(0,pixels,mask));memcpy(sprite,saved,46);
        rayhit hit; CHECK(AsmRefresh(0,&hit));
        CHECK(hit.side==2 && hit.x==6*65536+32768 && hit.column==31 && hit.page==1);
        /* Replace the closed door with its original wall tile for an exact normal-hit oracle. */
        tilemap[6][2]=1;
        CHECK(AsmRefresh(0,&hit));
        CHECK(hit.side==1 && hit.grid==6 && hit.x==6*65536 && hit.y==163622 && hit.column==31 && hit.page==1 && hit.height==171);
        for(int angle=0;angle<360;angle+=15) {
            player.angle=angle; CHECK(AsmRefresh(angle*10,&hit));
            CHECK(hit.column<64 && hit.page<PMSpriteStart && hit.height>0);
        }
        player.angle=90;CHECK(AsmRefresh(900,&hit));CHECK(hit.side==0 && hit.y==65536 && hit.page==0);
        player.angle=180;CHECK(AsmRefresh(1800,&hit));CHECK(hit.side==1 && hit.x==65536 && hit.page==1);
        player.angle=270;CHECK(AsmRefresh(2700,&hit));CHECK(hit.side==0 && hit.y==63*65536 && hit.page==0);
        player.angle=90;tilemap[2][1]=128;
        doorobjlist[0].vertical=false;doorobjlist[0].lock=2;
        CHECK(AsmRefresh(900,&hit));CHECK(hit.side==2 && hit.y==98304 && hit.page==6);
        tilemap[2][1]=0;
        player.angle=0;
        int32_t x=player.x,y=player.y; ControlMovement(0,-35,false);
        CHECK(player.x==x+5249 && player.y==y);
        player.x=x;player.y=y;ControlMovement(35,0,true); CHECK(player.x==x && player.y==y+5249);
        player.x=6*65536-MINDIST-100;player.y=y; ClipMove(&player,1000,1000);
        CHECK(player.x==6*65536-MINDIST-100 && player.y==y+1000);
        CHECK(!CA_CacheMap(60)); CHECK(CA_Map()); /* Rejected request preserves valid map. */
    } else {
        unsigned char pixels[4096],mask[4096];
        /* Every non-sparse Steam sprite is validated in place, without importing art. */
        for(unsigned sprite=0;sprite<PMSoundStart-PMSpriteStart;++sprite)
            if(PM_GetPage(PMSpriteStart+sprite,NULL)) CHECK(PM_DecodeSprite(sprite,pixels,mask));
    }
    ShutdownGameLevel();
}
static void TestMapBounds(void)
{
    CHECK(GameStart(0));
    /* Empty the perimeter so only coordinate bounds can reject these probes. */
    for(int i=0;i<64;++i) {
        const int edge_x[4]={0,63,i,i},edge_y[4]={i,i,0,63};
        for(unsigned edge=0;edge<4;++edge) {
            tilemap[edge_x[edge]][edge_y[edge]]=0;
            staticblock[edge_x[edge]][edge_y[edge]]=false;
            actorat[edge_x[edge]][edge_y[edge]]=NULL;
        }
    }
    objtype probe=player;
    const int32_t inside_x=3*TILEGLOBAL+32768,inside_y=5*TILEGLOBAL+32768;
    probe.x=inside_x;probe.y=inside_y;
    probe.x=MINDIST-1;CHECK(!TryMove(&probe));
    probe.x=MINDIST;CHECK(TryMove(&probe));
    probe.x=64*TILEGLOBAL-MINDIST;CHECK(!TryMove(&probe));
    probe.x=64*TILEGLOBAL-MINDIST-1;CHECK(TryMove(&probe));
    probe.x=inside_x;
    probe.y=MINDIST-1;CHECK(!TryMove(&probe));
    probe.y=MINDIST;CHECK(TryMove(&probe));
    probe.y=64*TILEGLOBAL-MINDIST;CHECK(!TryMove(&probe));
    probe.y=64*TILEGLOBAL-MINDIST-1;CHECK(TryMove(&probe));

    /* An off-map move is refused outright, not slid along an open axis. */
    player.x=63*TILEGLOBAL+32768;player.y=inside_y;
    int32_t x=player.x,y=player.y;
    ClipMove(&player,TILEGLOBAL/2,TILEGLOBAL/2);
    CHECK(player.x==x && player.y==y);
    player.x=32768;player.y=inside_y;x=player.x;y=player.y;
    ClipMove(&player,-TILEGLOBAL/2-1,-TILEGLOBAL/2);
    CHECK(player.x==x && player.y==y);
    GameStop();
}
static void TestDoors(void)
{
    CHECK(SetupGameLevel());
    doorobj_t *d=&doorobjlist[0];
    player.x=(int32_t)(d->x*65536)-MINDIST-100;
    player.y=(int32_t)(d->y*65536)+32768;player.tilex=(int)d->x-1;player.tiley=(int)d->y;player.angle=0;
    d->lock=0;d->vertical=true;
    Cmd_Use();CHECK(d->action==dr_opening && DoorSolid(0));
    for(unsigned i=0;i<63;++i) MoveDoors();
    CHECK(d->position==64512 && DoorSolid(0));
    actorat[d->x][d->y]=&objlist[0]; /* stale destination from a death */
    MoveDoors();CHECK(!actorat[d->x][d->y]);
    CHECK(d->position==65535 && !DoorSolid(0));
    int32_t x=player.x;ClipMove(&player,1000,0);CHECK(player.x==x+1000);
    player.tilex=(int)d->x;player.x=(int32_t)(d->x*65536)+32768;
    for(unsigned i=0;i<301;++i) MoveDoors();
    CHECK(d->action==dr_open && d->ticcount==301);
    d->ticcount=32767;MoveDoors();CHECK(d->ticcount==32768 && d->action==dr_open);
    d->ticcount=65535;MoveDoors();CHECK(d->ticcount==0 && d->action==dr_open);
    d->ticcount=301;
    player.tilex=(int)d->x-2;player.x=(int32_t)(d->x*65536)-2*65536;
    MoveDoors();CHECK(d->action==dr_closing && DoorSolid(0));
    for(unsigned i=0;i<64;++i) MoveDoors();
    CHECK(d->position==0 && d->action==dr_closed);
    d->lock=1;OperateDoor(0);CHECK(d->action==dr_closed);
    /* DOS checks the center row/column, not diagonal square corners. */
    int32_t left=(int32_t)d->x*65536,top=(int32_t)d->y*65536;
    for(unsigned vertical=0;vertical<2;++vertical) for(unsigned corner=0;corner<4;++corner) {
        d->vertical=vertical!=0;d->action=dr_open;d->position=65535;
        player.x=(corner&1) ? left+65536+MINDIST-1 : left-MINDIST;
        player.y=(corner&2) ? top+65536+MINDIST-1 : top-MINDIST;
        player.tilex=player.x/65536;player.tiley=player.y/65536;
        CloseDoor(0);CHECK(d->action==dr_closing);
        d->position=60000;MoveDoors();
        CHECK(d->action==dr_closing && d->position==58976);
    }
    for(unsigned vertical=0;vertical<2;++vertical) {
        d->vertical=vertical!=0;d->action=dr_open;d->position=65535;
        player.x=vertical ? left-MINDIST : left+32768;
        player.y=vertical ? top+32768 : top-MINDIST;
        player.tilex=player.x/65536;player.tiley=player.y/65536;
        CloseDoor(0);CHECK(d->action==dr_open);
        player.x=left+32768;player.y=top+32768;
        player.tilex=(int)d->x;player.tiley=(int)d->y;
        d->action=dr_closing;MoveDoors();CHECK(d->action==dr_opening);
    }
    player.x=left-MINDIST-1;player.y=top-MINDIST-1;
    player.tilex=player.x/65536;player.tiley=player.y/65536;
    d->action=dr_open;CloseDoor(0);CHECK(d->action==dr_closing);
    ShutdownGameLevel();
}
static void TestSimulation(void)
{
    CHECK(GameStart(0));CHECK(framebuffer[(VIEWY+VIEWHEIGHT)*320+VIEWX-1]==124); IN_KeyEvent(sc_UpArrow,true);
    for(uint64_t t=1;t<=9;++t) CHECK(GameStep(t));
    objtype fine=player;
    CHECK(GameStart(0)); IN_KeyEvent(sc_UpArrow,true);CHECK(GameStep(9));
    CHECK(player.x==fine.x && player.y==fine.y && player.angle==fine.angle);
    CHECK(GameStart(0)); IN_KeyEvent(sc_RightArrow,true);
    for(uint64_t t=1;t<=9;++t) CHECK(GameStep(t));
    fine=player;
    CHECK(GameStart(0));IN_KeyEvent(sc_RightArrow,true);CHECK(GameStep(9));CHECK(player.angle==fine.angle);
    CHECK(GameStart(0)); int32_t initial=player.x; Key(sc_UpArrow);
    CHECK(GameStep(0) && player.x==initial);CHECK(GameStep(1) && player.x>initial);
    initial=player.x;CHECK(GameStep(2) && player.x==initial);
    CHECK(GameStart(0));IN_KeyEvent(sc_UpArrow,true);IN_ClearKeysDown();initial=player.x;
    CHECK(GameStep(1) && player.x==initial);
    CHECK(GameStart(0));IN_KeyEvent(sc_LeftArrow,true);IN_KeyEvent(sc_RightArrow,true);
    CHECK(GameStep(3) && player.angle==0);
    CHECK(GameStart(0));IN_KeyEvent(sc_UpArrow,true);CHECK(GameStep(1000));fine=player;
    CHECK(GameStart(0));IN_KeyEvent(sc_UpArrow,true);CHECK(GameStep(10));CHECK(player.x==fine.x && player.y==fine.y);
    GameStop();
}
static void TestOwnedWalk(void)
{
    CHECK(GameStart(0));int start=player.tilex,door=0;
    for(int x=start+1;x<63;++x) if(tilemap[x][player.tiley]&128) { door=x;break; }
    CHECK(door);bool used=false;IN_KeyEvent(sc_UpArrow,true);
    for(uint64_t t=1;t<500 && player.tilex<door+2;++t) {
        if(!used && player.tilex+1==door) { Key(sc_Space);used=true; }
        CHECK(GameStep(t));
    }
    CHECK(used && player.tilex>=door+2);
    GameStop();
}
static void TestMenuGame(void)
{
    CHECK(GameProbe()); MainMenu[newgame].active=1; MainItems.curpos=newgame;
    CHECK(DemoLoopStartup(0,true) && DemoLoop(10)); Key(sc_Enter);
    CHECK(DemoLoop(11) && DemoLoop(31) && DemoLoopState()==UI_EPISODE);
    Key(sc_Enter);CHECK(DemoLoop(32) && DemoLoop(52) && DemoLoopState()==UI_DIFFICULTY);
    Key(sc_Enter);CHECK(DemoLoop(53) && DemoLoop(73) && DemoLoopState()==UI_GAME);
    TestFinishLoading(73);Key(sc_UpArrow); CHECK(DemoLoop(74));
    Key(sc_Escape); CHECK(DemoLoop(75) && DemoLoop(95) && DemoLoopState()==UI_MENU);
    CHECK(CA_Map() && ChunksInFile && GameRunning());
    GameStop();
    MainMenu[newgame].active=0;MainItems.curpos=viewscores;
}
int main(int argc,char **argv)
{
    CHECK(argc>=2);TestExpand();TestFixed();TestScale();MM_Startup();CHECK(!GameProbe());CHECK(CA_Startup(argv[1]));
    CHECK(VL_SetPalette(gamepal,768));TestInputMenu();TestWorld(argc==2,argc>3 ? argv[3] : NULL);TestMapBounds();if(argc==2) TestDoors();TestSimulation();if(argc>2) TestOwnedWalk();TestMenuGame();
    GameStop();CA_Shutdown();MM_Shutdown();CHECK(!MM_TotalBytes());
    puts("Original map/VSWAP, fixed-point ray/scaler, movement and menu/game tests passed.");return 0;
}
