//////////////////////////////////////
//
// Graphics .H file for .SOD
// IGRAB-ed on Thu Oct 08 20:38:29 1992
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
		BJCOLLAPSE1PIC,                      // 31
		BJCOLLAPSE2PIC,                      // 32
		BJCOLLAPSE3PIC,                      // 33
		BJCOLLAPSE4PIC,                      // 34
		ENDPICPIC,                           // 35
		// Lump Start
		L_GUYPIC,                            // 36
		L_COLONPIC,                          // 37
		L_NUM0PIC,                           // 38
		L_NUM1PIC,                           // 39
		L_NUM2PIC,                           // 40
		L_NUM3PIC,                           // 41
		L_NUM4PIC,                           // 42
		L_NUM5PIC,                           // 43
		L_NUM6PIC,                           // 44
		L_NUM7PIC,                           // 45
		L_NUM8PIC,                           // 46
		L_NUM9PIC,                           // 47
		L_PERCENTPIC,                        // 48
		L_APIC,                              // 49
		L_BPIC,                              // 50
		L_CPIC,                              // 51
		L_DPIC,                              // 52
		L_EPIC,                              // 53
		L_FPIC,                              // 54
		L_GPIC,                              // 55
		L_HPIC,                              // 56
		L_IPIC,                              // 57
		L_JPIC,                              // 58
		L_KPIC,                              // 59
		L_LPIC,                              // 60
		L_MPIC,                              // 61
		L_NPIC,                              // 62
		L_OPIC,                              // 63
		L_PPIC,                              // 64
		L_QPIC,                              // 65
		L_RPIC,                              // 66
		L_SPIC,                              // 67
		L_TPIC,                              // 68
		L_UPIC,                              // 69
		L_VPIC,                              // 70
		L_WPIC,                              // 71
		L_XPIC,                              // 72
		L_YPIC,                              // 73
		L_ZPIC,                              // 74
		L_EXPOINTPIC,                        // 75
		L_APOSTROPHEPIC,                     // 76
		L_GUY2PIC,                           // 77
		L_BJWINSPIC,                         // 78
		// Lump Start
		TITLE1PIC,                           // 79
		TITLE2PIC,                           // 80
		// Lump Start
		ENDSCREEN11PIC,                      // 81
		// Lump Start
		ENDSCREEN12PIC,                      // 82
		ENDSCREEN3PIC,                       // 83
		ENDSCREEN4PIC,                       // 84
		ENDSCREEN5PIC,                       // 85
		ENDSCREEN6PIC,                       // 86
		ENDSCREEN7PIC,                       // 87
		ENDSCREEN8PIC,                       // 88
		ENDSCREEN9PIC,                       // 89
		STATUSBARPIC,                        // 90
		PG13PIC,                             // 91
		CREDITSPIC,                          // 92
		// Lump Start
		IDGUYS1PIC,                          // 93
		IDGUYS2PIC,                          // 94
		// Lump Start
		COPYPROTTOPPIC,                      // 95
		COPYPROTBOXPIC,                      // 96
		BOSSPIC1PIC,                         // 97
		BOSSPIC2PIC,                         // 98
		BOSSPIC3PIC,                         // 99
		BOSSPIC4PIC,                         // 100
		// Lump Start
		KNIFEPIC,                            // 101
		GUNPIC,                              // 102
		MACHINEGUNPIC,                       // 103
		GATLINGGUNPIC,                       // 104
		NOKEYPIC,                            // 105
		GOLDKEYPIC,                          // 106
		SILVERKEYPIC,                        // 107
		N_BLANKPIC,                          // 108
		N_0PIC,                              // 109
		N_1PIC,                              // 110
		N_2PIC,                              // 111
		N_3PIC,                              // 112
		N_4PIC,                              // 113
		N_5PIC,                              // 114
		N_6PIC,                              // 115
		N_7PIC,                              // 116
		N_8PIC,                              // 117
		N_9PIC,                              // 118
		FACE1APIC,                           // 119
		FACE1BPIC,                           // 120
		FACE1CPIC,                           // 121
		FACE2APIC,                           // 122
		FACE2BPIC,                           // 123
		FACE2CPIC,                           // 124
		FACE3APIC,                           // 125
		FACE3BPIC,                           // 126
		FACE3CPIC,                           // 127
		FACE4APIC,                           // 128
		FACE4BPIC,                           // 129
		FACE4CPIC,                           // 130
		FACE5APIC,                           // 131
		FACE5BPIC,                           // 132
		FACE5CPIC,                           // 133
		FACE6APIC,                           // 134
		FACE6BPIC,                           // 135
		FACE6CPIC,                           // 136
		FACE7APIC,                           // 137
		FACE7BPIC,                           // 138
		FACE7CPIC,                           // 139
		FACE8APIC,                           // 140
		GOTGATLINGPIC,                       // 141
		GODMODEFACE1PIC,                     // 142
		GODMODEFACE2PIC,                     // 143
		GODMODEFACE3PIC,                     // 144
		BJWAITING1PIC,                       // 145
		BJWAITING2PIC,                       // 146
		BJOUCHPIC,                           // 147
		PAUSEDPIC,                           // 148
		GETPSYCHEDPIC,                       // 149



		ORDERSCREEN=151,
		ERRORSCREEN,                         // 152
		TITLEPALETTE,                        // 153
		END1PALETTE,                         // 154
		END2PALETTE,                         // 155
		END3PALETTE,                         // 156
		END4PALETTE,                         // 157
		END5PALETTE,                         // 158
		END6PALETTE,                         // 159
		END7PALETTE,                         // 160
		END8PALETTE,                         // 161
		END9PALETTE,                         // 162
		IDGUYSPALETTE,                       // 163
		T_DEMO0,                             // 164
		T_DEMO1,                             // 165
		T_DEMO2,                             // 166
		T_DEMO3,                             // 167
		T_ENDART1,                           // 168
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

#define ENDGAME_LUMP_START		31
#define ENDGAME_LUMP_END		35

#define LEVELEND_LUMP_START		36
#define LEVELEND_LUMP_END		78

#define TITLESCREEN_LUMP_START		79
#define TITLESCREEN_LUMP_END		80

#define ENDGAME1_LUMP_START		81
#define ENDGAME1_LUMP_END		81

#define ENDGAME2_LUMP_START		82
#define ENDGAME2_LUMP_END		82

#define EASTEREGG_LUMP_START		93
#define EASTEREGG_LUMP_END		94

#define COPYPROT_LUMP_START		95
#define COPYPROT_LUMP_END		100

#define LATCHPICS_LUMP_START		101
#define LATCHPICS_LUMP_END		149


//
// Amount of each data item
//
#define NUMCHUNKS    169
#define NUMFONT      2
#define NUMFONTM     0
#define NUMPICS      147
#define NUMPICM      0
#define NUMSPRITES   0
#define NUMTILE8     72
#define NUMTILE8M    0
#define NUMTILE16    0
#define NUMTILE16M   0
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   18
//
// File offsets for data items
//
#define STRUCTPIC    0

#define STARTFONT    1
#define STARTFONTM   3
#define STARTPICS    3
#define STARTPICM    150
#define STARTSPRITES 150
#define STARTTILE8   150
#define STARTTILE8M  151
#define STARTTILE16  151
#define STARTTILE16M 151
#define STARTTILE32  151
#define STARTTILE32M 151
#define STARTEXTERNS 151

//
// Thank you for using IGRAB!
//
