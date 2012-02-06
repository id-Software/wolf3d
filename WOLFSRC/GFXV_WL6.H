//////////////////////////////////////
//
// Graphics .H file for .WL6
// IGRAB-ed on Wed Apr 13 06:58:44 1994
//
//////////////////////////////////////

typedef enum {
		// Lump Start
		H_BJPIC=3,
		H_CASTLEPIC,                         // 4
		H_BLAZEPIC,                          // 5
		H_TOPWINDOWPIC,                      // 6
		H_LEFTWINDOWPIC,                     // 7
		H_RIGHTWINDOWPIC,                    // 8
		H_BOTTOMINFOPIC,                     // 9
		// Lump Start
		C_OPTIONSPIC,                        // 10
		C_CURSOR1PIC,                        // 11
		C_CURSOR2PIC,                        // 12
		C_NOTSELECTEDPIC,                    // 13
		C_SELECTEDPIC,                       // 14
		C_FXTITLEPIC,                        // 15
		C_DIGITITLEPIC,                      // 16
		C_MUSICTITLEPIC,                     // 17
		C_MOUSELBACKPIC,                     // 18
		C_BABYMODEPIC,                       // 19
		C_EASYPIC,                           // 20
		C_NORMALPIC,                         // 21
		C_HARDPIC,                           // 22
		C_LOADSAVEDISKPIC,                   // 23
		C_DISKLOADING1PIC,                   // 24
		C_DISKLOADING2PIC,                   // 25
		C_CONTROLPIC,                        // 26
		C_CUSTOMIZEPIC,                      // 27
		C_LOADGAMEPIC,                       // 28
		C_SAVEGAMEPIC,                       // 29
		C_EPISODE1PIC,                       // 30
		C_EPISODE2PIC,                       // 31
		C_EPISODE3PIC,                       // 32
		C_EPISODE4PIC,                       // 33
		C_EPISODE5PIC,                       // 34
		C_EPISODE6PIC,                       // 35
		C_CODEPIC,                           // 36
		C_TIMECODEPIC,                       // 37
		C_LEVELPIC,                          // 38
		C_NAMEPIC,                           // 39
		C_SCOREPIC,                          // 40
		C_JOY1PIC,                           // 41
		C_JOY2PIC,                           // 42
		// Lump Start
		L_GUYPIC,                            // 43
		L_COLONPIC,                          // 44
		L_NUM0PIC,                           // 45
		L_NUM1PIC,                           // 46
		L_NUM2PIC,                           // 47
		L_NUM3PIC,                           // 48
		L_NUM4PIC,                           // 49
		L_NUM5PIC,                           // 50
		L_NUM6PIC,                           // 51
		L_NUM7PIC,                           // 52
		L_NUM8PIC,                           // 53
		L_NUM9PIC,                           // 54
		L_PERCENTPIC,                        // 55
		L_APIC,                              // 56
		L_BPIC,                              // 57
		L_CPIC,                              // 58
		L_DPIC,                              // 59
		L_EPIC,                              // 60
		L_FPIC,                              // 61
		L_GPIC,                              // 62
		L_HPIC,                              // 63
		L_IPIC,                              // 64
		L_JPIC,                              // 65
		L_KPIC,                              // 66
		L_LPIC,                              // 67
		L_MPIC,                              // 68
		L_NPIC,                              // 69
		L_OPIC,                              // 70
		L_PPIC,                              // 71
		L_QPIC,                              // 72
		L_RPIC,                              // 73
		L_SPIC,                              // 74
		L_TPIC,                              // 75
		L_UPIC,                              // 76
		L_VPIC,                              // 77
		L_WPIC,                              // 78
		L_XPIC,                              // 79
		L_YPIC,                              // 80
		L_ZPIC,                              // 81
		L_EXPOINTPIC,                        // 82
		L_APOSTROPHEPIC,                     // 83
		L_GUY2PIC,                           // 84
		L_BJWINSPIC,                         // 85
		STATUSBARPIC,                        // 86
		TITLEPIC,                            // 87
		PG13PIC,                             // 88
		CREDITSPIC,                          // 89
		HIGHSCORESPIC,                       // 90
		// Lump Start
		KNIFEPIC,                            // 91
		GUNPIC,                              // 92
		MACHINEGUNPIC,                       // 93
		GATLINGGUNPIC,                       // 94
		NOKEYPIC,                            // 95
		GOLDKEYPIC,                          // 96
		SILVERKEYPIC,                        // 97
		N_BLANKPIC,                          // 98
		N_0PIC,                              // 99
		N_1PIC,                              // 100
		N_2PIC,                              // 101
		N_3PIC,                              // 102
		N_4PIC,                              // 103
		N_5PIC,                              // 104
		N_6PIC,                              // 105
		N_7PIC,                              // 106
		N_8PIC,                              // 107
		N_9PIC,                              // 108
		FACE1APIC,                           // 109
		FACE1BPIC,                           // 110
		FACE1CPIC,                           // 111
		FACE2APIC,                           // 112
		FACE2BPIC,                           // 113
		FACE2CPIC,                           // 114
		FACE3APIC,                           // 115
		FACE3BPIC,                           // 116
		FACE3CPIC,                           // 117
		FACE4APIC,                           // 118
		FACE4BPIC,                           // 119
		FACE4CPIC,                           // 120
		FACE5APIC,                           // 121
		FACE5BPIC,                           // 122
		FACE5CPIC,                           // 123
		FACE6APIC,                           // 124
		FACE6BPIC,                           // 125
		FACE6CPIC,                           // 126
		FACE7APIC,                           // 127
		FACE7BPIC,                           // 128
		FACE7CPIC,                           // 129
		FACE8APIC,                           // 130
		GOTGATLINGPIC,                       // 131
		MUTANTBJPIC,                         // 132
		PAUSEDPIC,                           // 133
		GETPSYCHEDPIC,                       // 134



		ORDERSCREEN=136,
		ERRORSCREEN,                         // 137
		T_HELPART,                           // 138
		T_DEMO0,                             // 139
		T_DEMO1,                             // 140
		T_DEMO2,                             // 141
		T_DEMO3,                             // 142
		T_ENDART1,                           // 143
		T_ENDART2,                           // 144
		T_ENDART3,                           // 145
		T_ENDART4,                           // 146
		T_ENDART5,                           // 147
		T_ENDART6,                           // 148
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define README_LUMP_START		3
#define README_LUMP_END			9

#define CONTROLS_LUMP_START		10
#define CONTROLS_LUMP_END		42

#define LEVELEND_LUMP_START		43
#define LEVELEND_LUMP_END		85

#define LATCHPICS_LUMP_START		91
#define LATCHPICS_LUMP_END		134


//
// Amount of each data item
//
#define NUMCHUNKS    149
#define NUMFONT      2
#define NUMFONTM     0
#define NUMPICS      132
#define NUMPICM      0
#define NUMSPRITES   0
#define NUMTILE8     72
#define NUMTILE8M    0
#define NUMTILE16    0
#define NUMTILE16M   0
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   13
//
// File offsets for data items
//
#define STRUCTPIC    0

#define STARTFONT    1
#define STARTFONTM   3
#define STARTPICS    3
#define STARTPICM    135
#define STARTSPRITES 135
#define STARTTILE8   135
#define STARTTILE8M  136
#define STARTTILE16  136
#define STARTTILE16M 136
#define STARTTILE32  136
#define STARTTILE32M 136
#define STARTEXTERNS 136

//
// Thank you for using IGRAB!
//
