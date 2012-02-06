//////////////////////////////////////
//
// Graphics .H file for .SDM
// IGRAB-ed on Mon Sep 21 14:20:19 1992
//
//////////////////////////////////////

typedef enum {
		// Lump Start
		C_BACKDROPPIC=3,
		C_MOUSELBACKPIC,                     // 4
		C_CURSOR1PIC,                        // 5
		C_CURSOR2PIC,                        // 6
		C_NOTSELECTEDPIC,                    // 7
		C_SELECTEDPIC,                       // 8
		// Lump Start
		C_CUSTOMIZEPIC,                      // 9
		C_JOY1PIC,                           // 10
		C_JOY2PIC,                           // 11
		C_MOUSEPIC,                          // 12
		C_JOYSTICKPIC,                       // 13
		C_KEYBOARDPIC,                       // 14
		C_CONTROLPIC,                        // 15
		// Lump Start
		C_OPTIONSPIC,                        // 16
		// Lump Start
		C_FXTITLEPIC,                        // 17
		C_DIGITITLEPIC,                      // 18
		C_MUSICTITLEPIC,                     // 19
		// Lump Start
		C_HOWTOUGHPIC,                       // 20
		C_BABYMODEPIC,                       // 21
		C_EASYPIC,                           // 22
		C_NORMALPIC,                         // 23
		C_HARDPIC,                           // 24
		// Lump Start
		C_DISKLOADING1PIC,                   // 25
		C_DISKLOADING2PIC,                   // 26
		C_LOADGAMEPIC,                       // 27
		C_SAVEGAMEPIC,                       // 28
		// Lump Start
		HIGHSCORESPIC,                       // 29
		C_WONSPEARPIC,                       // 30
		// Lump Start
		L_GUYPIC,                            // 31
		L_COLONPIC,                          // 32
		L_NUM0PIC,                           // 33
		L_NUM1PIC,                           // 34
		L_NUM2PIC,                           // 35
		L_NUM3PIC,                           // 36
		L_NUM4PIC,                           // 37
		L_NUM5PIC,                           // 38
		L_NUM6PIC,                           // 39
		L_NUM7PIC,                           // 40
		L_NUM8PIC,                           // 41
		L_NUM9PIC,                           // 42
		L_PERCENTPIC,                        // 43
		L_APIC,                              // 44
		L_BPIC,                              // 45
		L_CPIC,                              // 46
		L_DPIC,                              // 47
		L_EPIC,                              // 48
		L_FPIC,                              // 49
		L_GPIC,                              // 50
		L_HPIC,                              // 51
		L_IPIC,                              // 52
		L_JPIC,                              // 53
		L_KPIC,                              // 54
		L_LPIC,                              // 55
		L_MPIC,                              // 56
		L_NPIC,                              // 57
		L_OPIC,                              // 58
		L_PPIC,                              // 59
		L_QPIC,                              // 60
		L_RPIC,                              // 61
		L_SPIC,                              // 62
		L_TPIC,                              // 63
		L_UPIC,                              // 64
		L_VPIC,                              // 65
		L_WPIC,                              // 66
		L_XPIC,                              // 67
		L_YPIC,                              // 68
		L_ZPIC,                              // 69
		L_EXPOINTPIC,                        // 70
		L_APOSTROPHEPIC,                     // 71
		L_GUY2PIC,                           // 72
		L_BJWINSPIC,                         // 73
		// Lump Start
		TITLE1PIC,                           // 74
		TITLE2PIC,                           // 75
		STATUSBARPIC,                        // 76
		PG13PIC,                             // 77
		CREDITSPIC,                          // 78
		// Lump Start
		KNIFEPIC,                            // 79
		GUNPIC,                              // 80
		MACHINEGUNPIC,                       // 81
		GATLINGGUNPIC,                       // 82
		NOKEYPIC,                            // 83
		GOLDKEYPIC,                          // 84
		SILVERKEYPIC,                        // 85
		N_BLANKPIC,                          // 86
		N_0PIC,                              // 87
		N_1PIC,                              // 88
		N_2PIC,                              // 89
		N_3PIC,                              // 90
		N_4PIC,                              // 91
		N_5PIC,                              // 92
		N_6PIC,                              // 93
		N_7PIC,                              // 94
		N_8PIC,                              // 95
		N_9PIC,                              // 96
		FACE1APIC,                           // 97
		FACE1BPIC,                           // 98
		FACE1CPIC,                           // 99
		FACE2APIC,                           // 100
		FACE2BPIC,                           // 101
		FACE2CPIC,                           // 102
		FACE3APIC,                           // 103
		FACE3BPIC,                           // 104
		FACE3CPIC,                           // 105
		FACE4APIC,                           // 106
		FACE4BPIC,                           // 107
		FACE4CPIC,                           // 108
		FACE5APIC,                           // 109
		FACE5BPIC,                           // 110
		FACE5CPIC,                           // 111
		FACE6APIC,                           // 112
		FACE6BPIC,                           // 113
		FACE6CPIC,                           // 114
		FACE7APIC,                           // 115
		FACE7BPIC,                           // 116
		FACE7CPIC,                           // 117
		FACE8APIC,                           // 118
		GOTGATLINGPIC,                       // 119
		GODMODEFACE1PIC,                     // 120
		GODMODEFACE2PIC,                     // 121
		GODMODEFACE3PIC,                     // 122
		BJWAITING1PIC,                       // 123
		BJWAITING2PIC,                       // 124
		BJOUCHPIC,                           // 125
		PAUSEDPIC,                           // 126
		GETPSYCHEDPIC,                       // 127



		ORDERSCREEN=129,
		ERRORSCREEN,                         // 130
		TITLEPALETTE,                        // 131
		T_DEMO0,                             // 132
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define BACKDROP_LUMP_START		3
#define BACKDROP_LUMP_END		8

#define CONTROL_LUMP_START		9
#define CONTROL_LUMP_END		15

#define OPTIONS_LUMP_START		16
#define OPTIONS_LUMP_END		16

#define SOUND_LUMP_START		17
#define SOUND_LUMP_END			19

#define NEWGAME_LUMP_START		20
#define NEWGAME_LUMP_END		24

#define LOADSAVE_LUMP_START		25
#define LOADSAVE_LUMP_END		28

#define HIGHSCORES_LUMP_START		29
#define HIGHSCORES_LUMP_END		30

#define LEVELEND_LUMP_START		31
#define LEVELEND_LUMP_END		73

#define TITLESCREEN_LUMP_START		74
#define TITLESCREEN_LUMP_END		75

#define LATCHPICS_LUMP_START		79
#define LATCHPICS_LUMP_END		127


//
// Amount of each data item
//
#define NUMCHUNKS    133
#define NUMFONT      2
#define NUMFONTM     0
#define NUMPICS      125
#define NUMPICM      0
#define NUMSPRITES   0
#define NUMTILE8     72
#define NUMTILE8M    0
#define NUMTILE16    0
#define NUMTILE16M   0
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   4
//
// File offsets for data items
//
#define STRUCTPIC    0

#define STARTFONT    1
#define STARTFONTM   3
#define STARTPICS    3
#define STARTPICM    128
#define STARTSPRITES 128
#define STARTTILE8   128
#define STARTTILE8M  129
#define STARTTILE16  129
#define STARTTILE16M 129
#define STARTTILE32  129
#define STARTTILE32M 129
#define STARTEXTERNS 129

//
// Thank you for using IGRAB!
//
