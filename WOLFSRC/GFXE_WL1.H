//////////////////////////////////////
//
// Graphics .H file for .WL1
// IGRAB-ed on Sat May 02 02:16:56 1992
//
//////////////////////////////////////

typedef enum {
		// Lump Start
		H_BJPIC=3,
		H_CASTLEPIC,                 // 4
		H_KEYBOARDPIC,               // 5
		H_JOYPIC,                    // 6
		H_MOUSEPIC,                  // 7
		H_BOTHPIC,                   // 8
		H_GAMEPADPIC,                // 9
		H_HEALPIC,                   // 10
		H_TREASUREPIC,               // 11
		H_GUNPIC,                    // 12
		H_KEYPIC,                    // 13
		H_BLAZEPIC,                  // 14
		H_WEAPON1234PIC,             // 15
		H_WOLFLOGOPIC,               // 16
		H_VISAPIC,                   // 17
		H_MCPIC,                     // 18
		H_IDLOGOPIC,                 // 19
		H_FAXPIC,                    // 20
		H_GALACTIXPIC,               // 21
		H_FGODMOMPIC,                // 22
		H_AUSTRALIAPIC,              // 23
		H_CANADAPIC,                 // 24
		H_UKPIC,                     // 25
		// Lump Start
		C_OPTIONSPIC,                // 26
		C_CURSOR1PIC,                // 27
		C_CURSOR2PIC,                // 28
		C_NOTSELECTEDPIC,            // 29
		C_SELECTEDPIC,               // 30
		C_FXTITLEPIC,                // 31
		C_DIGITITLEPIC,              // 32
		C_MUSICTITLEPIC,             // 33
		C_MOUSELBACKPIC,             // 34
		C_BABYMODEPIC,               // 35
		C_EASYPIC,                   // 36
		C_NORMALPIC,                 // 37
		C_HARDPIC,                   // 38
		C_LOADSAVEDISKPIC,           // 39
		C_DISKLOADING1PIC,           // 40
		C_DISKLOADING2PIC,           // 41
		C_CONTROLPIC,                // 42
		C_CUSTOMIZEPIC,              // 43
		C_LOADGAMEPIC,               // 44
		C_SAVEGAMEPIC,               // 45
		C_EPISODE1PIC,               // 46
		C_EPISODE2PIC,               // 47
		C_EPISODE3PIC,               // 48
		C_EPISODE4PIC,               // 49
		C_EPISODE5PIC,               // 50
		C_EPISODE6PIC,               // 51
		C_CODEPIC,                   // 52
		// Lump Start
		L_GUYPIC,                    // 53
		L_COLONPIC,                  // 54
		L_NUM0PIC,                   // 55
		L_NUM1PIC,                   // 56
		L_NUM2PIC,                   // 57
		L_NUM3PIC,                   // 58
		L_NUM4PIC,                   // 59
		L_NUM5PIC,                   // 60
		L_NUM6PIC,                   // 61
		L_NUM7PIC,                   // 62
		L_NUM8PIC,                   // 63
		L_NUM9PIC,                   // 64
		L_PERCENTPIC,                // 65
		L_APIC,                      // 66
		L_BPIC,                      // 67
		L_CPIC,                      // 68
		L_DPIC,                      // 69
		L_EPIC,                      // 70
		L_FPIC,                      // 71
		L_GPIC,                      // 72
		L_HPIC,                      // 73
		L_IPIC,                      // 74
		L_JPIC,                      // 75
		L_KPIC,                      // 76
		L_LPIC,                      // 77
		L_MPIC,                      // 78
		L_NPIC,                      // 79
		L_OPIC,                      // 80
		L_PPIC,                      // 81
		L_QPIC,                      // 82
		L_RPIC,                      // 83
		L_SPIC,                      // 84
		L_TPIC,                      // 85
		L_UPIC,                      // 86
		L_VPIC,                      // 87
		L_WPIC,                      // 88
		L_XPIC,                      // 89
		L_YPIC,                      // 90
		L_ZPIC,                      // 91
		L_GUY2PIC,                   // 92
		L_BJWINSPIC,                 // 93
		STATUSBARPIC,                // 94
		TITLEPIC,                    // 95
		PG13PIC,                     // 96
		CREDITSPIC,                  // 97
		HIGHSCORESPIC,               // 98
		// Lump Start
		KNIFEPIC,                    // 99
		GUNPIC,                      // 100
		MACHINEGUNPIC,               // 101
		GATLINGGUNPIC,               // 102
		NOKEYPIC,                    // 103
		GOLDKEYPIC,                  // 104
		SILVERKEYPIC,                // 105
		N_BLANKPIC,                  // 106
		N_0PIC,                      // 107
		N_1PIC,                      // 108
		N_2PIC,                      // 109
		N_3PIC,                      // 110
		N_4PIC,                      // 111
		N_5PIC,                      // 112
		N_6PIC,                      // 113
		N_7PIC,                      // 114
		N_8PIC,                      // 115
		N_9PIC,                      // 116
		FACE1APIC,                   // 117
		FACE1BPIC,                   // 118
		FACE1CPIC,                   // 119
		FACE2APIC,                   // 120
		FACE2BPIC,                   // 121
		FACE2CPIC,                   // 122
		FACE3APIC,                   // 123
		FACE3BPIC,                   // 124
		FACE3CPIC,                   // 125
		FACE4APIC,                   // 126
		FACE4BPIC,                   // 127
		FACE4CPIC,                   // 128
		FACE5APIC,                   // 129
		FACE5BPIC,                   // 130
		FACE5CPIC,                   // 131
		FACE6APIC,                   // 132
		FACE6BPIC,                   // 133
		FACE6CPIC,                   // 134
		FACE7APIC,                   // 135
		FACE7BPIC,                   // 136
		FACE7CPIC,                   // 137
		FACE8APIC,                   // 138
		GOTGATLINGPIC,               // 139
		PAUSEDPIC,                   // 140
		GETPSYCHEDPIC,               // 141



		ORDERSCREEN=557,
		ENUMEND
	     } graphicnums;

//
// Data LUMPs
//
#define README_LUMP_START		3
#define README_LUMP_END			25

#define CONTROLS_LUMP_START		26
#define CONTROLS_LUMP_END		52

#define LEVELEND_LUMP_START		53
#define LEVELEND_LUMP_END		93

#define LATCHPICS_LUMP_START		99
#define LATCHPICS_LUMP_END		141


//
// Amount of each data item
//
#define NUMCHUNKS    558
#define NUMFONT      2
#define NUMFONTM     0
#define NUMPICS      139
#define NUMPICM      0
#define NUMSPRITES   0
#define NUMTILE8     72
#define NUMTILE8M    0
#define NUMTILE16    144
#define NUMTILE16M   270
#define NUMTILE32    0
#define NUMTILE32M   0
#define NUMEXTERNS   1
//
// File offsets for data items
//
#define STRUCTPIC    0

#define STARTFONT    1
#define STARTFONTM   3
#define STARTPICS    3
#define STARTPICM    142
#define STARTSPRITES 142
#define STARTTILE8   142
#define STARTTILE8M  143
#define STARTTILE16  143
#define STARTTILE16M 287
#define STARTTILE32  557
#define STARTTILE32M 557
#define STARTEXTERNS 557

//
// Thank you for using IGRAB!
//
