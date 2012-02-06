// WL_INTER.C

#include "WL_DEF.H"
#pragma hdrstop


//==========================================================================

/*
==================
=
= CLearSplitVWB
=
==================
*/

void ClearSplitVWB (void)
{
	memset (update,0,sizeof(update));
	WindowX = 0;
	WindowY = 0;
	WindowW = 320;
	WindowH = 160;
}


//==========================================================================

#ifdef SPEAR
#ifndef SPEARDEMO
////////////////////////////////////////////////////////
//
// End of Spear of Destiny
//
////////////////////////////////////////////////////////

void EndScreen (int palette, int screen)
{
	CA_CacheScreen (screen);
	VW_UpdateScreen ();
	CA_CacheGrChunk (palette);
	VL_FadeIn(0,255,grsegs[palette],30);
	UNCACHEGRCHUNK (palette);
	IN_ClearKeysDown ();
	IN_Ack ();
	VW_FadeOut ();
}


void EndSpear(void)
{
	EndScreen (END1PALETTE, ENDSCREEN11PIC);

	CA_CacheScreen (ENDSCREEN3PIC);
	VW_UpdateScreen ();
	CA_CacheGrChunk (END3PALETTE);
	VL_FadeIn(0,255,grsegs[END3PALETTE],30);
	UNCACHEGRCHUNK (END3PALETTE);
	fontnumber = 0;
	fontcolor = 0xd0;
	WindowX = 0;
	WindowW = 320;
	PrintX = 0;
	PrintY = 180;
	US_CPrint (STR_ENDGAME1"\n");
	US_CPrint (STR_ENDGAME2);
	VW_UpdateScreen ();
	IN_StartAck ();
	TimeCount = 0;
	while (!IN_CheckAck () && TimeCount < 700);

	PrintX = 0;
	PrintY = 180;
	VWB_Bar(0,180,320,20,0);
	US_CPrint (STR_ENDGAME3"\n");
	US_CPrint (STR_ENDGAME4);
	VW_UpdateScreen ();
	IN_StartAck ();
	TimeCount = 0;
	while (!IN_CheckAck () && TimeCount < 700);

	VW_FadeOut ();

	EndScreen (END4PALETTE, ENDSCREEN4PIC);
	EndScreen (END5PALETTE, ENDSCREEN5PIC);
	EndScreen (END6PALETTE, ENDSCREEN6PIC);
	EndScreen (END7PALETTE, ENDSCREEN7PIC);
	EndScreen (END8PALETTE, ENDSCREEN8PIC);
	EndScreen (END9PALETTE, ENDSCREEN9PIC);

	EndScreen (END2PALETTE, ENDSCREEN12PIC);

	MainMenu[savegame].active = 0;
}
#endif
#endif

//==========================================================================

/*
==================
=
= Victory
=
==================
*/

void Victory (void)
{
#ifndef SPEARDEMO
	long	sec;
	int i,min,kr,sr,tr,x;
	char tempstr[8];

#define RATIOX	6
#define RATIOY	14
#define TIMEX	14
#define TIMEY	8


#ifdef SPEAR
	StartCPMusic (XTHEEND_MUS);

	CA_CacheGrChunk(BJCOLLAPSE1PIC);
	CA_CacheGrChunk(BJCOLLAPSE2PIC);
	CA_CacheGrChunk(BJCOLLAPSE3PIC);
	CA_CacheGrChunk(BJCOLLAPSE4PIC);

	VWB_Bar(0,0,320,200,VIEWCOLOR);
	VWB_DrawPic (124,44,BJCOLLAPSE1PIC);
	VW_UpdateScreen ();
	VW_FadeIn ();
	VW_WaitVBL(2*70);
	VWB_DrawPic (124,44,BJCOLLAPSE2PIC);
	VW_UpdateScreen ();
	VW_WaitVBL(105);
	VWB_DrawPic (124,44,BJCOLLAPSE3PIC);
	VW_UpdateScreen ();
	VW_WaitVBL(105);
	VWB_DrawPic (124,44,BJCOLLAPSE4PIC);
	VW_UpdateScreen ();
	VW_WaitVBL(3*70);

	UNCACHEGRCHUNK(BJCOLLAPSE1PIC);
	UNCACHEGRCHUNK(BJCOLLAPSE2PIC);
	UNCACHEGRCHUNK(BJCOLLAPSE3PIC);
	UNCACHEGRCHUNK(BJCOLLAPSE4PIC);
	VL_FadeOut (0,255,0,17,17,5);
#endif

	StartCPMusic (URAHERO_MUS);
	ClearSplitVWB ();
	CacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
	CA_CacheGrChunk(STARTFONT);

#ifndef SPEAR
	CA_CacheGrChunk(C_TIMECODEPIC);
#endif


	VWB_Bar (0,0,320,200-STATUSLINES,127);
#ifdef JAPAN
#ifndef JAPDEMO
	CA_CacheGrChunk(C_ENDRATIOSPIC);
	VWB_DrawPic(0,0,C_ENDRATIOSPIC);
	UNCACHEGRCHUNK(C_ENDRATIOSPIC);
#endif
#else
	Write(18,2,STR_YOUWIN);

	Write(TIMEX,TIMEY-2,STR_TOTALTIME);

	Write(12,RATIOY-2,"averages");

	#ifdef SPANISH
	Write(RATIOX+2,  RATIOY,      STR_RATKILL);
	Write(RATIOX+2,  RATIOY+2,  STR_RATSECRET);
	Write(RATIOX+2,  RATIOY+4,STR_RATTREASURE);
	#else
	Write(RATIOX+8,RATIOY,      STR_RATKILL);
	Write(RATIOX+4,RATIOY+2,  STR_RATSECRET);
	Write(RATIOX,  RATIOY+4,STR_RATTREASURE);
	#endif

#endif

#ifndef JAPDEMO
	VWB_DrawPic (8,4,L_BJWINSPIC);
#endif


#ifndef SPEAR
	for (kr = sr = tr = sec = i = 0;i < 8;i++)
#else
	for (kr = sr = tr = sec = i = 0;i < 20;i++)
#endif
	{
		sec += LevelRatios[i].time;
		kr += LevelRatios[i].kill;
		sr += LevelRatios[i].secret;
		tr += LevelRatios[i].treasure;
	}

#ifndef SPEAR
	kr /= 8;
	sr /= 8;
	tr /= 8;
#else
	kr /= 14;
	sr /= 14;
	tr /= 14;
#endif

	min = sec/60;
	sec %= 60;

	if (min > 99)
		min = sec = 99;

	i = TIMEX*8+1;
	VWB_DrawPic(i,TIMEY*8,L_NUM0PIC+(min/10));
	i += 2*8;
	VWB_DrawPic(i,TIMEY*8,L_NUM0PIC+(min%10));
	i += 2*8;
	Write(i/8,TIMEY,":");
	i += 1*8;
	VWB_DrawPic(i,TIMEY*8,L_NUM0PIC+(sec/10));
	i += 2*8;
	VWB_DrawPic(i,TIMEY*8,L_NUM0PIC+(sec%10));
	VW_UpdateScreen ();

	itoa(kr,tempstr,10);
	x=RATIOX+24-strlen(tempstr)*2;
	Write(x,RATIOY,tempstr);

	itoa(sr,tempstr,10);
	x=RATIOX+24-strlen(tempstr)*2;
	Write(x,RATIOY+2,tempstr);

	itoa(tr,tempstr,10);
	x=RATIOX+24-strlen(tempstr)*2;
	Write(x,RATIOY+4,tempstr);


#ifndef SPANISH
#ifndef UPLOAD
#ifndef SPEAR
	//
	// TOTAL TIME VERIFICATION CODE
	//
	if (gamestate.difficulty>=gd_medium)
	{
		VWB_DrawPic (30*8,TIMEY*8,C_TIMECODEPIC);
		fontnumber = 0;
		fontcolor = READHCOLOR;
		PrintX = 30*8-3;
		PrintY = TIMEY*8+8;
		PrintX+=4;
		tempstr[0] = (((min/10)^(min%10))^0xa)+'A';
		tempstr[1] = (((sec/10)^(sec%10))^0xa)+'A';
		tempstr[2] = (tempstr[0]^tempstr[1])+'A';
		tempstr[3] = 0;
		US_Print(tempstr);
	}
#endif
#endif
#endif


	fontnumber = 1;

	VW_UpdateScreen ();
	VW_FadeIn ();

	IN_Ack();

	#ifndef SPEAR
	if (Keyboard[sc_P] && MS_CheckParm("goobers"))
		PicturePause();
	#endif

	VW_FadeOut ();

#ifndef SPEAR
	UNCACHEGRCHUNK(C_TIMECODEPIC);
#endif
	UnCacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);

#ifndef SPEAR
	EndText();
#else
	EndSpear();
#endif

#endif // SPEARDEMO
}


//==========================================================================

#ifndef JAPAN
/*
==================
=
= PG13
=
==================
*/

void PG13 (void)
{
	VW_FadeOut();
	VWB_Bar(0,0,320,200,0x82);			// background

	CA_CacheGrChunk (PG13PIC);
	VWB_DrawPic (216,110,PG13PIC);
	VW_UpdateScreen ();

	UNCACHEGRCHUNK (PG13PIC);

	VW_FadeIn();
	IN_UserInput(TickBase*7);

	VW_FadeOut ();
}
#endif


//==========================================================================

void Write(int x,int y,char *string)
{
 int alpha[]={L_NUM0PIC,L_NUM1PIC,L_NUM2PIC,L_NUM3PIC,L_NUM4PIC,L_NUM5PIC,
	L_NUM6PIC,L_NUM7PIC,L_NUM8PIC,L_NUM9PIC,L_COLONPIC,0,0,0,0,0,0,L_APIC,L_BPIC,
	L_CPIC,L_DPIC,L_EPIC,L_FPIC,L_GPIC,L_HPIC,L_IPIC,L_JPIC,L_KPIC,
	L_LPIC,L_MPIC,L_NPIC,L_OPIC,L_PPIC,L_QPIC,L_RPIC,L_SPIC,L_TPIC,
	L_UPIC,L_VPIC,L_WPIC,L_XPIC,L_YPIC,L_ZPIC};

 int i,ox,nx,ny;
 char ch;


 ox=nx=x*8;
 ny=y*8;
 for (i=0;i<strlen(string);i++)
   if (string[i]=='\n')
   {
	nx=ox;
	ny+=16;
   }
   else
   {
	ch=string[i];
	if (ch>='a')
	  ch-=('a'-'A');
	ch-='0';

	switch(string[i])
	{
	 case '!':
	   VWB_DrawPic(nx,ny,L_EXPOINTPIC);
	   nx+=8;
	   continue;

	 case '\'':
	   VWB_DrawPic(nx,ny,L_APOSTROPHEPIC);
	   nx+=8;
	   continue;

	 case ' ': break;
	 case 0x3a:	// ':'

	   VWB_DrawPic(nx,ny,L_COLONPIC);
	   nx+=8;
	   continue;

	 case '%':
	   VWB_DrawPic(nx,ny,L_PERCENTPIC);
	   break;

	 default:
	   VWB_DrawPic(nx,ny,alpha[ch]);
	}
	nx+=16;
   }
}


//
// Breathe Mr. BJ!!!
//
void BJ_Breathe(void)
{
	static int which=0,max=10;
	int pics[2]={L_GUYPIC,L_GUY2PIC};


	if (TimeCount>max)
	{
		which^=1;
		VWB_DrawPic(0,16,pics[which]);
		VW_UpdateScreen();
		TimeCount=0;
		max=35;
	}
}



/*
==================
=
= LevelCompleted
=
= Entered with the screen faded out
= Still in split screen mode with the status bar
=
= Exit with the screen faded out
=
==================
*/

#ifndef SPEAR
LRstruct LevelRatios[8];
#else
LRstruct LevelRatios[20];
#endif

void LevelCompleted (void)
{
	#define VBLWAIT	30
	#define PAR_AMOUNT	500
	#define PERCENT100AMT	10000
	typedef struct {
			float time;
			char timestr[6];
			} times;

	int	x,i,min,sec,ratio,kr,sr,tr;
	unsigned	temp;
	char tempstr[10];
	long bonus,timeleft=0;
	times parTimes[]=
	{
#ifndef SPEAR
	 //
	 // Episode One Par Times
	 //
	 {1.5,	"01:30"},
	 {2,	"02:00"},
	 {2,	"02:00"},
	 {3.5,	"03:30"},
	 {3,	"03:00"},
	 {3,	"03:00"},
	 {2.5,	"02:30"},
	 {2.5,	"02:30"},
	 {0,	"??:??"},	// Boss level
	 {0,	"??:??"},	// Secret level

	 //
	 // Episode Two Par Times
	 //
	 {1.5,	"01:30"},
	 {3.5,	"03:30"},
	 {3,	"03:00"},
	 {2,	"02:00"},
	 {4,	"04:00"},
	 {6,	"06:00"},
	 {1,	"01:00"},
	 {3,	"03:00"},
	 {0,	"??:??"},
	 {0,	"??:??"},

	 //
	 // Episode Three Par Times
	 //
	 {1.5,	"01:30"},
	 {1.5,	"01:30"},
	 {2.5,	"02:30"},
	 {2.5,	"02:30"},
	 {3.5,	"03:30"},
	 {2.5,	"02:30"},
	 {2,	"02:00"},
	 {6,	"06:00"},
	 {0,	"??:??"},
	 {0,	"??:??"},

	 //
	 // Episode Four Par Times
	 //
	 {2,	"02:00"},
	 {2,	"02:00"},
	 {1.5,	"01:30"},
	 {1,	"01:00"},
	 {4.5,	"04:30"},
	 {3.5,	"03:30"},
	 {2,	"02:00"},
	 {4.5,	"04:30"},
	 {0,	"??:??"},
	 {0,	"??:??"},

	 //
	 // Episode Five Par Times
	 //
	 {2.5,	"02:30"},
	 {1.5,	"01:30"},
	 {2.5,	"02:30"},
	 {2.5,	"02:30"},
	 {4,	"04:00"},
	 {3,	"03:00"},
	 {4.5,	"04:30"},
	 {3.5,	"03:30"},
	 {0,	"??:??"},
	 {0,	"??:??"},

	 //
	 // Episode Six Par Times
	 //
	 {6.5,	"06:30"},
	 {4,	"04:00"},
	 {4.5,	"04:30"},
	 {6,	"06:00"},
	 {5,	"05:00"},
	 {5.5,	"05:30"},
	 {5.5,	"05:30"},
	 {8.5,	"08:30"},
	 {0,	"??:??"},
	 {0,	"??:??"}
#else
	 //
	 // SPEAR OF DESTINY TIMES
	 //
	 {1.5,	"01:30"},
	 {3.5,	"03:30"},
	 {2.75,	"02:45"},
	 {3.5,	"03:30"},
	 {0,	"??:??"},	// Boss 1
	 {4.5,	"04:30"},
	 {3.25,	"03:15"},
	 {2.75,	"02:45"},
	 {4.75,	"04:45"},
	 {0,	"??:??"},	// Boss 2
	 {6.5,	"06:30"},
	 {4.5,	"04:30"},
	 {2.75,	"02:45"},
	 {4.5,	"04:30"},
	 {6,	"06:00"},
	 {0,	"??:??"},	// Boss 3
	 {6,	"06:00"},
	 {0,	"??:??"},	// Boss 4
	 {0,	"??:??"},	// Secret level 1
	 {0,	"??:??"},	// Secret level 2
#endif
	};



	CacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
	ClearSplitVWB ();			// set up for double buffering in split screen
	VWB_Bar (0,0,320,200-STATUSLINES,127);
	StartCPMusic(ENDLEVEL_MUS);

//
// do the intermission
//
	IN_ClearKeysDown();
	IN_StartAck();

#ifdef JAPAN
	CA_CacheGrChunk(C_INTERMISSIONPIC);
	VWB_DrawPic(0,0,C_INTERMISSIONPIC);
	UNCACHEGRCHUNK(C_INTERMISSIONPIC);
#endif
	VWB_DrawPic(0,16,L_GUYPIC);

#ifndef SPEAR
	if (mapon<8)
#else
	if (mapon != 4 &&
		mapon != 9 &&
		mapon != 15 &&
		mapon < 17)
#endif
	{
#ifndef JAPAN
	 #ifdef SPANISH
	 Write(14,2,"piso\ncompletado");
	 #else
	 Write(14,2,"floor\ncompleted");
	 #endif

	 Write(14,7,STR_BONUS"     0");
	 Write(16,10,STR_TIME);
	 Write(16,12,STR_PAR);

	 #ifdef SPANISH
	 Write(11,14,    STR_RAT2KILL);
	 Write(11,16,  STR_RAT2SECRET);
	 Write(11,18,STR_RAT2TREASURE);
	 #else
	 Write(9,14,    STR_RAT2KILL);
	 Write(5,16,  STR_RAT2SECRET);
	 Write(1,18,STR_RAT2TREASURE);
	 #endif

	 Write(26,2,itoa(gamestate.mapon+1,tempstr,10));
#endif

	 #ifdef SPANISH
	 Write(30,12,parTimes[gamestate.episode*10+mapon].timestr);
	 #else
	 Write(26,12,parTimes[gamestate.episode*10+mapon].timestr);
	 #endif

	 //
	 // PRINT TIME
	 //
	 sec=gamestate.TimeCount/70;

	 if (sec > 99*60)		// 99 minutes max
	   sec = 99*60;

	 if (gamestate.TimeCount<parTimes[gamestate.episode*10+mapon].time*4200)
		timeleft=(parTimes[gamestate.episode*10+mapon].time*4200)/70-sec;

	 min=sec/60;
	 sec%=60;

	 #ifdef SPANISH
	 i=30*8;
	 #else
	 i=26*8;
	 #endif
	 VWB_DrawPic(i,10*8,L_NUM0PIC+(min/10));
	 i+=2*8;
	 VWB_DrawPic(i,10*8,L_NUM0PIC+(min%10));
	 i+=2*8;
	 Write(i/8,10,":");
	 i+=1*8;
	 VWB_DrawPic(i,10*8,L_NUM0PIC+(sec/10));
	 i+=2*8;
	 VWB_DrawPic(i,10*8,L_NUM0PIC+(sec%10));

	 VW_UpdateScreen ();
	 VW_FadeIn ();


	 //
	 // FIGURE RATIOS OUT BEFOREHAND
	 //
	 kr = sr = tr = 0;
	 if (gamestate.killtotal)
		kr=(gamestate.killcount*100)/gamestate.killtotal;
	 if (gamestate.secrettotal)
		sr=(gamestate.secretcount*100)/gamestate.secrettotal;
	 if (gamestate.treasuretotal)
		tr=(gamestate.treasurecount*100)/gamestate.treasuretotal;


	 //
	 // PRINT TIME BONUS
	 //
	 bonus=timeleft*PAR_AMOUNT;
	 if (bonus)
	 {
	  for (i=0;i<=timeleft;i++)
	  {
	   ltoa((long)i*PAR_AMOUNT,tempstr,10);
	   x=36-strlen(tempstr)*2;
	   Write(x,7,tempstr);
	   if (!(i%(PAR_AMOUNT/10)))
		 SD_PlaySound(ENDBONUS1SND);
	   VW_UpdateScreen();
	   while(SD_SoundPlaying())
		 BJ_Breathe();
	   if (IN_CheckAck())
		 goto done;
	  }

	  VW_UpdateScreen();
	  SD_PlaySound(ENDBONUS2SND);
	  while(SD_SoundPlaying())
		BJ_Breathe();
	 }


	 #ifdef SPANISH
	 #define RATIOXX		33
	 #else
	 #define RATIOXX		37
	 #endif
	 //
	 // KILL RATIO
	 //
	 ratio=kr;
	 for (i=0;i<=ratio;i++)
	 {
	  itoa(i,tempstr,10);
	  x=RATIOXX-strlen(tempstr)*2;
	  Write(x,14,tempstr);
	  if (!(i%10))
		SD_PlaySound(ENDBONUS1SND);
	  VW_UpdateScreen ();
	  while(SD_SoundPlaying())
		BJ_Breathe();

	  if (IN_CheckAck())
		goto done;
	 }
	 if (ratio==100)
	 {
	   VW_WaitVBL(VBLWAIT);
	   SD_StopSound();
	   bonus+=PERCENT100AMT;
	   ltoa(bonus,tempstr,10);
	   x=(RATIOXX-1)-strlen(tempstr)*2;
	   Write(x,7,tempstr);
	   VW_UpdateScreen();
	   SD_PlaySound(PERCENT100SND);
	 }
	 else
	 if (!ratio)
	 {
	   VW_WaitVBL(VBLWAIT);
	   SD_StopSound();
	   SD_PlaySound(NOBONUSSND);
	 }
	 else
	 SD_PlaySound(ENDBONUS2SND);

	 VW_UpdateScreen();
	 while(SD_SoundPlaying())
	   BJ_Breathe();


	 //
	 // SECRET RATIO
	 //
	 ratio=sr;
	 for (i=0;i<=ratio;i++)
	 {
	  itoa(i,tempstr,10);
	  x=RATIOXX-strlen(tempstr)*2;
	  Write(x,16,tempstr);
	  if (!(i%10))
		SD_PlaySound(ENDBONUS1SND);
	  VW_UpdateScreen ();
	  while(SD_SoundPlaying())
		BJ_Breathe();
	  BJ_Breathe();

	  if (IN_CheckAck())
		goto done;
	 }
	 if (ratio==100)
	 {
	   VW_WaitVBL(VBLWAIT);
	   SD_StopSound();
	   bonus+=PERCENT100AMT;
	   ltoa(bonus,tempstr,10);
	   x=(RATIOXX-1)-strlen(tempstr)*2;
	   Write(x,7,tempstr);
	   VW_UpdateScreen();
	   SD_PlaySound(PERCENT100SND);
	 }
	 else
	 if (!ratio)
	 {
	   VW_WaitVBL(VBLWAIT);
	   SD_StopSound();
	   SD_PlaySound(NOBONUSSND);
	 }
	 else
	   SD_PlaySound(ENDBONUS2SND);
	 VW_UpdateScreen();
	 while(SD_SoundPlaying())
	   BJ_Breathe();


	 //
	 // TREASURE RATIO
	 //
	 ratio=tr;
	 for (i=0;i<=ratio;i++)
	 {
	  itoa(i,tempstr,10);
	  x=RATIOXX-strlen(tempstr)*2;
	  Write(x,18,tempstr);
	  if (!(i%10))
		SD_PlaySound(ENDBONUS1SND);
	  VW_UpdateScreen ();
	  while(SD_SoundPlaying())
		BJ_Breathe();
	  if (IN_CheckAck())
		goto done;
	 }
	 if (ratio==100)
	 {
	   VW_WaitVBL(VBLWAIT);
	   SD_StopSound();
	   bonus+=PERCENT100AMT;
	   ltoa(bonus,tempstr,10);
	   x=(RATIOXX-1)-strlen(tempstr)*2;
	   Write(x,7,tempstr);
	   VW_UpdateScreen();
	   SD_PlaySound(PERCENT100SND);
	 }
	 else
	 if (!ratio)
	 {
	   VW_WaitVBL(VBLWAIT);
	   SD_StopSound();
	   SD_PlaySound(NOBONUSSND);
	 }
	 else
	 SD_PlaySound(ENDBONUS2SND);
	 VW_UpdateScreen();
	 while(SD_SoundPlaying())
	   BJ_Breathe();


	 //
	 // JUMP STRAIGHT HERE IF KEY PRESSED
	 //
	 done:

	 itoa(kr,tempstr,10);
	 x=RATIOXX-strlen(tempstr)*2;
	 Write(x,14,tempstr);

	 itoa(sr,tempstr,10);
	 x=RATIOXX-strlen(tempstr)*2;
	 Write(x,16,tempstr);

	 itoa(tr,tempstr,10);
	 x=RATIOXX-strlen(tempstr)*2;
	 Write(x,18,tempstr);

	 bonus=(long)timeleft*PAR_AMOUNT+
		   (PERCENT100AMT*(kr==100))+
		   (PERCENT100AMT*(sr==100))+
		   (PERCENT100AMT*(tr==100));

	 GivePoints(bonus);
	 ltoa(bonus,tempstr,10);
	 x=36-strlen(tempstr)*2;
	 Write(x,7,tempstr);

	 //
	 // SAVE RATIO INFORMATION FOR ENDGAME
	 //
	 LevelRatios[mapon].kill=kr;
	 LevelRatios[mapon].secret=sr;
	 LevelRatios[mapon].treasure=tr;
	 LevelRatios[mapon].time=min*60+sec;
	}
	else
	{
#ifdef SPEAR
#ifndef SPEARDEMO
	  switch(mapon)
	  {
	   case 4: Write(14,4," trans\n"
						  " grosse\n"
						  STR_DEFEATED); break;
	   case 9: Write(14,4,"barnacle\n"
						  "wilhelm\n"
						  STR_DEFEATED); break;
	   case 15: Write(14,4,"ubermutant\n"
						   STR_DEFEATED); break;
	   case 17: Write(14,4," death\n"
						   " knight\n"
						   STR_DEFEATED); break;
	   case 18: Write(13,4,"secret tunnel\n"
						   "    area\n"
						   "  completed!"); break;
	   case 19: Write(13,4,"secret castle\n"
						   "    area\n"
						   "  completed!"); break;
	  }
#endif
#else
	  Write(14,4,"secret floor\n completed!");
#endif

	  Write(10,16,"15000 bonus!");

	  VW_UpdateScreen();
	  VW_FadeIn();

	  GivePoints(15000);
	}


	DrawScore();
	VW_UpdateScreen();

	TimeCount=0;
	IN_StartAck();
	while(!IN_CheckAck())
	  BJ_Breathe();

//
// done
//
#ifdef SPEARDEMO
	if (gamestate.mapon == 1)
	{
		SD_PlaySound (BONUS1UPSND);

		CA_CacheGrChunk (STARTFONT+1);
		Message ("This concludes your demo\n"
				 "of Spear of Destiny! Now,\n"
				 "go to your local software\n"
				 "store and buy it!");
		UNCACHEGRCHUNK (STARTFONT+1);

		IN_ClearKeysDown();
		IN_Ack();
	}
#endif

#ifdef JAPDEMO
	if (gamestate.mapon == 3)
	{
		SD_PlaySound (BONUS1UPSND);

		CA_CacheGrChunk (STARTFONT+1);
		Message ("This concludes your demo\n"
				 "of Wolfenstein 3-D! Now,\n"
				 "go to your local software\n"
				 "store and buy it!");
		UNCACHEGRCHUNK (STARTFONT+1);

		IN_ClearKeysDown();
		IN_Ack();
	}
#endif

	#ifndef SPEAR
	if (Keyboard[sc_P] && MS_CheckParm("goobers"))
		PicturePause();
	#endif

	VW_FadeOut ();
	temp = bufferofs;
	for (i=0;i<3;i++)
	{
		bufferofs = screenloc[i];
		DrawPlayBorder ();
	}
	bufferofs = temp;

	UnCacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
}



//==========================================================================


/*
=================
=
= PreloadGraphics
=
= Fill the cache up
=
=================
*/

boolean PreloadUpdate(unsigned current, unsigned total)
{
	unsigned w = WindowW - 10;


	VWB_Bar(WindowX + 5,WindowY + WindowH - 3,w,2,BLACK);
	w = ((long)w * current) / total;
	if (w)
	{
	 VWB_Bar(WindowX + 5,WindowY + WindowH - 3,w,2,0x37); //SECONDCOLOR);
	 VWB_Bar(WindowX + 5,WindowY + WindowH - 3,w-1,1,0x32);

	}
	VW_UpdateScreen();
//	if (LastScan == sc_Escape)
//	{
//		IN_ClearKeysDown();
//		return(true);
//	}
//	else
		return(false);
}

void PreloadGraphics(void)
{
	DrawLevel ();
	ClearSplitVWB ();			// set up for double buffering in split screen

	VWB_Bar (0,0,320,200-STATUSLINES,127);

	LatchDrawPic (20-14,80-3*8,GETPSYCHEDPIC);

	WindowX = 160-14*8;
	WindowY = 80-3*8;
	WindowW = 28*8;
	WindowH = 48;
	VW_UpdateScreen();
	VW_FadeIn ();

	PM_Preload (PreloadUpdate);
	IN_UserInput (70);
	VW_FadeOut ();

	DrawPlayBorder ();
	VW_UpdateScreen ();
}


//==========================================================================

/*
==================
=
= DrawHighScores
=
==================
*/

void	DrawHighScores(void)
{
	char		buffer[16],*str,buffer1[5];
	byte		temp,temp1,temp2,temp3;
	word		i,j,
				w,h,
				x,y;
	HighScore	*s;


	MM_SortMem ();

#ifndef SPEAR
//	CA_CacheGrChunk (C_CODEPIC);
	CA_CacheGrChunk (HIGHSCORESPIC);
	CA_CacheGrChunk (STARTFONT);
	CA_CacheGrChunk (C_LEVELPIC);
	CA_CacheGrChunk (C_SCOREPIC);
	CA_CacheGrChunk (C_NAMEPIC);

	ClearMScreen();
	DrawStripes(10);

	VWB_DrawPic(48,0,HIGHSCORESPIC);
	UNCACHEGRCHUNK (HIGHSCORESPIC);

	VWB_DrawPic(4*8,68,C_NAMEPIC);
	VWB_DrawPic(20*8,68,C_LEVELPIC);
	VWB_DrawPic(28*8,68,C_SCOREPIC);
#ifndef UPLOAD
//	VWB_DrawPic(35*8,68,C_CODEPIC);
#endif
	fontnumber=0;

#else
	CacheLump (BACKDROP_LUMP_START,BACKDROP_LUMP_END);
	ClearMScreen();
	DrawStripes(10);
	UnCacheLump (BACKDROP_LUMP_START,BACKDROP_LUMP_END);

	CacheLump (HIGHSCORES_LUMP_START,HIGHSCORES_LUMP_END);
	CA_CacheGrChunk (STARTFONT+1);
	VWB_DrawPic (0,0,HIGHSCORESPIC);

	fontnumber = 1;
#endif


#ifndef SPEAR
	SETFONTCOLOR(15,0x29);
#else
	SETFONTCOLOR(HIGHLIGHT,0x29);
#endif

	for (i = 0,s = Scores;i < MaxScores;i++,s++)
	{
		PrintY = 76 + (16 * i);

		//
		// name
		//
#ifndef SPEAR
		PrintX = 4*8;
#else
		PrintX = 16;
#endif
		US_Print(s->name);

		//
		// level
		//
		ultoa(s->completed,buffer,10);
#ifndef SPEAR
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');	// Used fixed-width numbers (129...)
		USL_MeasureString(buffer,&w,&h);
		PrintX = (22 * 8)-w;
#else
		USL_MeasureString(buffer,&w,&h);
		PrintX = 194 - w;
#endif

#ifndef UPLOAD
#ifndef SPEAR
		PrintX -= 6;
		itoa(s->episode+1,buffer1,10);
		US_Print("E");
		US_Print(buffer1);
		US_Print("/L");
#endif
#endif

#ifdef SPEAR
		if (s->completed == 21)
			VWB_DrawPic (PrintX+8,PrintY-1,C_WONSPEARPIC);
		else
#endif
		US_Print(buffer);

		//
		// score
		//
		ultoa(s->score,buffer,10);
#ifndef SPEAR
		for (str = buffer;*str;str++)
			*str = *str + (129 - '0');	// Used fixed-width numbers (129...)
		USL_MeasureString(buffer,&w,&h);
		PrintX = (34 * 8) - 8 - w;
#else
		USL_MeasureString(buffer,&w,&h);
		PrintX = 292 - w;
#endif
		US_Print(buffer);

		#if 0
#ifndef UPLOAD
#ifndef SPEAR
		//
		// verification #
		//
		if (!i)
		{
		 temp=(((s->score >> 28)& 0xf)^
			  ((s->score >> 24)& 0xf))+'A';
		 temp1=(((s->score >> 20)& 0xf)^
			   ((s->score >> 16)& 0xf))+'A';
		 temp2=(((s->score >> 12)& 0xf)^
			   ((s->score >> 8)& 0xf))+'A';
		 temp3=(((s->score >> 4)& 0xf)^
			   ((s->score >> 0)& 0xf))+'A';

		 SETFONTCOLOR(0x49,0x29);
		 PrintX = 35*8;
		 buffer[0]=temp;
		 buffer[1]=temp1;
		 buffer[2]=temp2;
		 buffer[3]=temp3;
		 buffer[4]=0;
		 US_Print(buffer);
		 SETFONTCOLOR(15,0x29);
		}
#endif
#endif
		#endif
	}

	VW_UpdateScreen ();

#ifdef SPEAR
	UnCacheLump (HIGHSCORES_LUMP_START,HIGHSCORES_LUMP_END);
	fontnumber = 0;
#endif
}

//===========================================================================


/*
=======================
=
= CheckHighScore
=
=======================
*/

void	CheckHighScore (long score,word other)
{
	word		i,j;
	int			n;
	HighScore	myscore;

	strcpy(myscore.name,"");
	myscore.score = score;
	myscore.episode = gamestate.episode;
	myscore.completed = other;

	for (i = 0,n = -1;i < MaxScores;i++)
	{
		if
		(
			(myscore.score > Scores[i].score)
		||	(
				(myscore.score == Scores[i].score)
			&& 	(myscore.completed > Scores[i].completed)
			)
		)
		{
			for (j = MaxScores;--j > i;)
				Scores[j] = Scores[j - 1];
			Scores[i] = myscore;
			n = i;
			break;
		}
	}

#ifdef SPEAR
	StartCPMusic (XAWARD_MUS);
#else
	StartCPMusic (ROSTER_MUS);
#endif
	DrawHighScores ();

	VW_FadeIn ();

	if (n != -1)
	{
	//
	// got a high score
	//
		PrintY = 76 + (16 * n);
#ifndef SPEAR
		PrintX = 4*8;
		backcolor = BORDCOLOR;
		fontcolor = 15;
		US_LineInput(PrintX,PrintY,Scores[n].name,nil,true,MaxHighName,100);
#else
		PrintX = 16;
		fontnumber = 1;
		VWB_Bar (PrintX-2,PrintY-2,145,15,0x9c);
		VW_UpdateScreen ();
		backcolor = 0x9c;
		fontcolor = 15;
		US_LineInput(PrintX,PrintY,Scores[n].name,nil,true,MaxHighName,130);
#endif
	}
	else
	{
		IN_ClearKeysDown ();
		IN_UserInput(500);
	}

}


#ifndef UPLOAD
#ifndef SPEAR
#ifndef JAPAN
////////////////////////////////////////////////////////
//
// NON-SHAREWARE NOTICE
//
////////////////////////////////////////////////////////
void NonShareware(void)
{
	VW_FadeOut();

	ClearMScreen();
	DrawStripes(10);

	CA_CacheGrChunk(STARTFONT+1);
	fontnumber = 1;

	SETFONTCOLOR(READHCOLOR,BKGDCOLOR);
	PrintX=110;
	PrintY=15;

	#ifdef SPANISH
	US_Print("Atencion");
	#else
	US_Print("Attention");
	#endif

	SETFONTCOLOR(HIGHLIGHT,BKGDCOLOR);
	WindowX=PrintX=40;
	PrintY=60;
	#ifdef SPANISH
	US_Print("Este juego NO es gratis y\n");
	US_Print("NO es Shareware; favor de\n");
	US_Print("no distribuirlo.\n\n");
	#else
	US_Print("This game is NOT shareware.\n");
	US_Print("Please do not distribute it.\n");
	US_Print("Thanks.\n\n");
	#endif
	US_Print("        Id Software\n");

	VW_UpdateScreen ();
	VW_FadeIn();
	IN_Ack();
}
#endif
#endif
#endif

#ifdef SPEAR
#ifndef SPEARDEMO
////////////////////////////////////////////////////////
//
// COPY PROTECTION FOR FormGen
//
////////////////////////////////////////////////////////
char 	far CopyProFailedStrs[][100] = {
			STR_COPY1,
			STR_COPY2,

			STR_COPY3,
			STR_COPY4,

			STR_COPY5,
			STR_COPY6,

			STR_COPY7,
			STR_COPY8,

			STR_COPY9,
			"",

			STR_COPY10,
			STR_COPY11,

			STR_COPY12,
			"",

			STR_COPY13,
			"",

			STR_COPY14,
			""
			},

		far BackDoorStrs[5][16] = {
			"a spoon?",
			"bite me!",
			"joshua",
			"pelt",
#ifdef BETA
			"beta"
#else
			"snoops"
#endif
			},

		far GoodBoyStrs[10][40] = {
			"...is the CORRECT ANSWER!",
			"",

			"Consider yourself bitten, sir.",
			"",

			"Greetings Professor Falken, would you",
			"like to play Spear of Destiny?",

			"Do you have any gold spray paint?",
			"",

#ifdef BETA
			"Beta testing approved.",
#else
			"I wish I had a 21\" monitor...",
#endif
			""
			},

		far bossstrs[4][24] = {
			"DEATH KNIGHT",
			"BARNACLE WILHELM",
			"UBERMUTANTUBER MUTANT",
			"TRANS GROSSE"
			},

		far WordStr[5][20] = {
			"New Game",
			"Sound...F4",
			"Control...F6",
			"Change View...F5",
			"Quit...F10"},

		far	WordCorrect[5][2] = {"3","4","4","5","5"},

		far MemberStr[10][40] = {
			STR_COPY15,
			"",

			STR_COPY16,
			"",

			STR_COPY17,
			STR_COPY18,

			STR_COPY19,
			STR_COPY20,

			STR_COPY21,
			STR_COPY22},

		far MemberCorrect[5][24] = {
			"adrian carmack",
			"john carmackjohn romero",
			"tom hall",
			"jay wilbur",
			"kevin cloud"},

		far DosMessages[9][80] = {
			STR_NOPE1,
			STR_NOPE2,
			STR_NOPE3,
			STR_NOPE4,
			STR_NOPE5,
			STR_NOPE6,
			STR_NOPE7,
			STR_NOPE8,
			STR_NOPE9},

		far MiscTitle[4][20] = {
			"BLOOD TEST",
			"STRAIGHT-LACED",
			"QUITE SHAPELY",
			"I AM WHAT I AMMO"
			},

		far MiscStr[12][40] = {
			STR_MISC1,
			STR_MISC2,
			"",

			STR_MISC3,
			STR_MISC4,
			"",

			STR_MISC5,
			STR_MISC6,
			"",

			STR_MISC7,
			STR_MISC8,
			STR_MISC9
			},

		far MiscCorrect[4][5] = {"ss","8",STR_STAR,"45"};


int  BackDoor(char *s)
{
	int i;


	strlwr(s);

	for (i=0;i<5;i++)
		if (!_fstrcmp(s,BackDoorStrs[i]))
		{
			SETFONTCOLOR(14,15);
			fontnumber = 0;
			PrintY = 175;
			VWB_DrawPic (0,20*8,COPYPROTBOXPIC);
			US_CPrint(GoodBoyStrs[i*2]);
			US_CPrint(GoodBoyStrs[i*2+1]);
			VW_UpdateScreen();
			return 1;
		}

	return 0;
}


void CopyProtection(void)
{
#define TYPEBOX_Y		177
#define TYPEBOX_BKGD	0x9c
#define PRINTCOLOR		HIGHLIGHT

	int	i,match,whichboss,bossnum,try,whichline,enemypicked[4]={0,0,0,0},
		bosses[4] = { BOSSPIC1PIC,BOSSPIC2PIC,BOSSPIC3PIC,BOSSPIC4PIC },
		whichone,whichpicked[4]={0,0,0,0},quiztype,whichmem,
		memberpicked[5]={0,0,0,0,0},wordpicked[5]={0,0,0,0,0},whichword;

	char	inputbuffer[20],
			message[80];

	enum
	{
		debriefing,
		checkmanual,
		staffquiz,
		miscquiz,

		totaltypes
	};



	try = 0;
	VW_FadeOut();
	CA_CacheGrChunk(C_BACKDROPPIC);
	CacheLump(COPYPROT_LUMP_START,COPYPROT_LUMP_END);
	CA_CacheGrChunk(STARTFONT+1);
	CA_LoadAllSounds();
	StartCPMusic(COPYPRO_MUS);
	US_InitRndT(true);

	while (try<3)
	{
		fontnumber = 1;
		SETFONTCOLOR(PRINTCOLOR-2,15);
		VWB_DrawPic (0,0,C_BACKDROPPIC);
		VWB_DrawPic (0,0,COPYPROTTOPPIC);
		VWB_DrawPic (0,20*8,COPYPROTBOXPIC);
		WindowX = WindowY = 0;
		WindowW = 320;
		WindowH = 200;
		PrintY = 65;

		quiztype = US_RndT()%totaltypes;
		switch(quiztype)
		{
			//
			// BOSSES QUIZ
			//
			case debriefing:
				PrintX = 0;
				US_Print(STR_DEBRIEF);
				SETFONTCOLOR(PRINTCOLOR,15);

				while (enemypicked[whichboss = US_RndT()&3]);
				enemypicked[whichboss] = 1;
				bossnum = bosses[whichboss];
				VWB_DrawPic(128,60,bossnum);
				fontnumber = 0;
				PrintY = 130;
				US_CPrint(STR_ENEMY1"\n");
				US_CPrint(STR_ENEMY2"\n\n");

				VW_UpdateScreen();
				VW_FadeIn();

				PrintX = 100;
				fontcolor = 15;
				backcolor = TYPEBOX_BKGD;
				inputbuffer[0] = 0;
				PrintY = TYPEBOX_Y;
				fontnumber = 1;
				US_LineInput(PrintX,PrintY,inputbuffer,nil,true,20,100);

				match = 0;
				for (i=0;i<_fstrlen(bossstrs[whichboss]);i++)
					if (!_fstrnicmp(inputbuffer,bossstrs[whichboss]+i,strlen(inputbuffer)) &&
						strlen(inputbuffer)>3)
						match = 1;

				match += BackDoor(inputbuffer);
				break;

			//
			// MANUAL CHECK
			//
			case checkmanual:
				while (wordpicked[whichword = US_RndT()%5]);
				wordpicked[whichword] = 1;
				US_CPrint(STR_CHECKMAN);
				SETFONTCOLOR(PRINTCOLOR,15);
				PrintY += 25;
				US_CPrint(STR_MAN1);
				US_CPrint(STR_MAN2);
				_fstrcpy(message,STR_MAN3" \"");
				_fstrcat(message,WordStr[whichword]);
				_fstrcat(message,"\" "STR_MAN4);
				US_CPrint(message);
				VW_UpdateScreen();
				VW_FadeIn();

				PrintX = 146;
				fontcolor = 15;
				backcolor = TYPEBOX_BKGD;
				inputbuffer[0] = 0;
				PrintY = TYPEBOX_Y;
				US_LineInput(PrintX,PrintY,inputbuffer,nil,true,6,100);

				strlwr(inputbuffer);
				match = 1-(_fstrcmp(inputbuffer,WordCorrect[whichword])!=0);
				match += BackDoor(inputbuffer);
				break;

			//
			// STAFF QUIZ
			//
			case staffquiz:
				while (memberpicked[whichmem = US_RndT()%5]);
				memberpicked[whichmem] = 1;
				US_CPrint(STR_ID1);
				SETFONTCOLOR(PRINTCOLOR,15);
				PrintY += 25;
				US_CPrint(MemberStr[whichmem*2]);
				US_CPrint(MemberStr[whichmem*2+1]);
				VW_UpdateScreen();
				VW_FadeIn();

				PrintX = 100;
				fontcolor = 15;
				backcolor = TYPEBOX_BKGD;
				inputbuffer[0] = 0;
				PrintY = TYPEBOX_Y;
				US_LineInput(PrintX,PrintY,inputbuffer,nil,true,20,120);

				strlwr(inputbuffer);
				match = 0;
				for (i=0;i<_fstrlen(MemberCorrect[whichmem]);i++)
					if (!_fstrnicmp(inputbuffer,MemberCorrect[whichmem]+i,strlen(inputbuffer)) &&
						strlen(inputbuffer)>2)
							match = 1;
				match += BackDoor(inputbuffer);
				break;

			//
			// MISCELLANEOUS QUESTIONS
			//
			case miscquiz:
				while (whichpicked[whichone = US_RndT()&3]);
				whichpicked[whichone] = 1;
				US_CPrint(MiscTitle[whichone]);
				SETFONTCOLOR(PRINTCOLOR,15);
				PrintY += 25;
				US_CPrint(MiscStr[whichone*3]);
				US_CPrint(MiscStr[whichone*3+1]);
				US_CPrint(MiscStr[whichone*3+2]);
				VW_UpdateScreen();
				VW_FadeIn();

				PrintX = 146;
				fontcolor = 15;
				backcolor = TYPEBOX_BKGD;
				inputbuffer[0] = 0;
				PrintY = TYPEBOX_Y;
				US_LineInput(PrintX,PrintY,inputbuffer,nil,true,6,100);

				strlwr(inputbuffer);
				match = 1-(_fstrcmp(inputbuffer,MiscCorrect[whichone])!=0);
				match += BackDoor(inputbuffer);
				break;
			}

		//
		// IF NO MATCH, WE'VE GOT A (MINOR) PROBLEM!
		//

		if (!match)
		{
			whichline = 2*(US_RndT()%9);
			SETFONTCOLOR(14,15);
			fontnumber = 0;
			PrintY = 175;
			VWB_DrawPic (0,20*8,COPYPROTBOXPIC);
			US_CPrint(CopyProFailedStrs[whichline]);
			US_CPrint(CopyProFailedStrs[whichline+1]);

			VW_UpdateScreen();
			SD_PlaySound(NOWAYSND);
			IN_UserInput(TickBase*3);
			VW_FadeOut();
			try++;
		}
		else
		{
			int start;


			SD_PlaySound(BONUS1UPSND);
			SD_WaitSoundDone();
			UNCACHEGRCHUNK (STARTFONT+1);
			UNCACHEGRCHUNK (C_BACKDROPPIC);
			UnCacheLump (COPYPROT_LUMP_START,COPYPROT_LUMP_END);

			switch(SoundMode)
			{
				case sdm_Off: return;
				case sdm_PC: start = STARTPCSOUNDS; break;
				case sdm_AdLib: start = STARTADLIBSOUNDS;
			}

			for (i=0;i<NUMSOUNDS;i++,start++)
				MM_FreePtr ((memptr *)&audiosegs[start]);
			return;
		}
	}

	ClearMemory();
	ShutdownId();

	_fstrcpy(message,DosMessages[US_RndT()%9]);

	_AX = 3;
	geninterrupt(0x10);

	printf("%s\n",message);
	exit(1);
}

#endif // SPEARDEMO
#endif // SPEAR
//===========================================================================
