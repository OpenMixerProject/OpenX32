#pragma once

const char*	OffOn[] = {" OFF", " ON", ""};
const char* Xamxgrp[] = {" OFF", " X", " Y", ""};
const char* Xcolors[] = {" OFF", " RD", " GN", " YE", " BL", " MG", " CY", " WH", " OFFi", " RDi", " GNi", " YEi", " BLi", " MGi", " CYi", " WHi", ""};
const char* XSsourc[] = {" OFF", " LR", " LR+C", " LRPFL", " LRAFL", " AUX56", " AUX78", ""};
const char* Xmnmode[] = {" LR+M", " LCR", ""};
const char* Xchmode[] = {" PFL", " AFL", ""};
const char*  Xhslop[] = {" 12", " 18", " 24", ""};
const char*  Xgmode[] = {" EXP2", " EXP3", " EXP", " GATE", " DUCK", ""};
const char* Xdymode[] = {" COMP", " EXP", ""};
const char*  Xdydet[] = {" PEAK", " RMS", ""};
const char*  Xdyenv[] = {" LIN", " LOG", ""};
const char*  Xdyrat[] = {" 1.1", " 1.3", " 1.5", " 2.0", " 2.5", " 3.0", " 4.0", " 5.0", " 7.0", " 10", " 20", " 100", ""};
const char* Xdyftyp[] = {" LC6", " LC12", " HC6", " HC12", " 1.0", " 2.0", " 3.0", " 5.0", " 10.0", ""};
const char* Xdyppos[] = {" PRE", " POST", ""};
const char*   Xisel[] = {" OFF", " FX1L", " FX1R", " FX2L", " FX2R", " FX3L", " FX3R",
				" FX4L", " FX4R", " FX5L", " FX5R", " FX6L", " FX6R", " FX7L",
				" FX7R", " FX8L", " FX8R", " AUX1", " AUX2", " AUX3", " AUX4",
				" AUX5", " AUX6", ""};
const char*  Xeqty1[] = {" LCut", " LShv", " PEQ", " VEQ", " HShv", " HCut", ""};
const char*  Xeqty2[] = {" LCut", " LShv", " PEQ", " VEQ", " HShv", " HCut", " BU6",
				" BU12", " BS12", " LR12", " BU18", " BU24", " BS24", " LR24", ""};
const char*  Xmtype[] = {" IN/LC", " <-EQ", " EQ->", " PRE", " POST", " GRP", ""};
const char* XTsourc[] = {" INT", " EXT", ""};
const char* XOscsel[] = {" F1", " F2", ""};
const char* XOsctyp[] = {" SINE", " PINK", " WHITE", ""};
const char*  XCFrsw[] = {" REC", " PLAY", ""};
const char*  XRtgin[] = {" AN1-8", " AN9-16", " AN17-24", " AN25-32", " A1-8", " A9-16", " A17-24", " A25-32", " A33-40",
				" A41-48", " B1-8", " B9-16", " B17-24",	" B25-32", " B33-40", " B41-48", " CARD1-8", " CARD9-16",
				" CARD17-24", " CARD25-32", ""};
const char*  XRtaea[] = {" AN1-8", " AN9-16", "AN17-24", " AN25-32", " A1-8",	" A9-16", " A17-24", " A25-32", " A33-40",
				" A41-48", " B1-8", " B9-16", " B17-24",	" B25-32", " B33-40", " B41-48", " CARD1-8", " CARD9-16",
				" CARD17-24", " CARD25-32", " OUT1-8", " OUT9-16", " P161-8", " P169-16", " AUX1-6/Mon", " AuxIN1-6/TB", ""};
const char*  XRtina[] = {" AUX1-4", " AN1-2", " AN1-4", " AN1-6", " A1-2", " A1-4", " A1-6",
				" B1-2", " B1-4", " B1-6", " CARD1-2", " CARD1-4", " CARD1-6", ""};
const char*  XRout1[] = {" AN1-4", " AN9-12", " AN17-20", " AN25-28", " A1-4", " A9-12", " A17-20", " A25-28", " A33-36",
				" A41-44" ," B1-4", " B9-12", " B17-20", " B25-28", " B33-36", " B41-44", " CARD1-4", " CARD9-12",
				" CARD17-20", " CARD25-28", " OUT1-4", " OUT9-12", " P161-4", " P169-12", " AUX/CR", " AUX/TB", ""};
const char*  XRout5[] = {" AN5-8", " AN13-16", " AN21-24", " AN29-32", " A5-8", " A13-16", " A21-24", " A29-32", " A37-40",
				" A45-48" ," B5-8", " B13-16", " B21-24", " B29-32", " B37-40", " B45-48", " CARD5-8", " CARD13-16",
				" CARD21-24", " CARD29-32", " OUT5-8", " OUT13-16", " P165-8", " P1613-16", " AUX/CR", " AUX/TB", ""};

const char* Sfxtyp1[] = {" HALL", " AMBI", " RPLT", " ROOM", " CHAM", " PLAT", " VREV", " VRM",
				" GATE", " RVRS", " DLY", " 3TAP", " 4TAP", " CRS", " FLNG", " PHAS", " DIMC", " FILT",
				" ROTA", " PAN", " SUB", " D/RV", " CR/R", " FL/R", " D/CR", " D/FL", " MODD", " GEQ2",
				" TEQ2", " GEQ", " TEQ", " DES2", " DES", " P1A2", " P1A", " PQ5S", " PQ5", " WAVD",
				" LIM", " CMB2", " CMB", " FAC2", " FAC1M", " FAC", " LEC2", " LEC", " ULC2", " ULC",
				" ENH2", " ENH", " EXC2", " EXC", " IMG", " EDI", " SON", " AMP2", " AMP", " DRV2",
				" DRV", " PIT2", " PIT", ""};

enum Sfxtyp1 {_1_HALL, _1_AMBI, _1_RPLT, _1_ROOM, _1_CHAM, _1_PLAT, _1_VREV, _1_VRM,
			_1_GATE, _1_RVRS, _1_DLY, _1_3TAP, _1_4TAP, _1_CRS, _1_FLNG, _1_PHAS, _1_DIMC, _1_FILT,
			_1_ROTA, _1_PAN, _1_SUB, _1_D_RV, _1_CR_R, _1_FL_R, _1_D_CR, _1_D_FL, _1_MODD, _1_GEQ2,
			_1_TEQ2, _1_GEQ, _1_TEQ, _1_DES2, _1_DES, _1_P1A2, _1_P1A, _1_PQ5S, _1_PQ5, _1_WAVD,
			_1_LIM, _1_CMB, _1_CMB2, _1_FAC2, _1_FAC1M, _1_FAC, _1_LEC2, _1_LEC, _1_ULC2, _1_ULC,
			_1_ENH2, _1_ENH, _1_EXC2, _1_EXC, _1_IMG, _1_EDI, _1_SON, _1_AMP2, _1_AMP, _1_DRV2,
			_1_DRV, _1_PIT2, _1_PIT};

const char* Sfxtyp2[] = {" GEQ2", " TEQ2", " GEQ", " TEQ", " DES2", " DES", " P1A", " P1A2" ,
				" PQ5", " PQ5S", " WAVD", " LIM", " FAC", " FAC1M", " FAC2", " LEC", " LEC2", " ULC" ,
				" ULC2", " ENH2", " ENH", " EXC2", " EXC", " IMG", " EDI", " SON", " AMP2", " AMP" ,
				" DRV2", " DRV", " PHAS", " FILT", " PAN", " SUB", ""};

enum Sfxtyp2 {_2_GEQ2 = _1_PIT + 2, _2_TEQ2, _2_GEQ, _2_TEQ, _2_DES2, _2_DES, _2_P1A, _2_P1A2,
			_2_PQ5S, _2_PQ5, _2_WAVD, _2_LIM, _2_FAC2, _2_FAC1M, _2_FAC, _2_LEC2, _2_LEC, _2_ULC2,
			_2_ULC, _2_ENH2, _2_ENH, _2_EXC2, _2_EXC, _2_IMG, _2_EDI, _2_SON, _2_AMP2, _2_AMP,
			_2_DRV2, _2_DRV, _2_PHAS, _2_FILT, _2_PAN, _2_SUB};

const char*  Sfxsrc[] = {" INS", " MIX1", " MIX2", " MIX3", " MIX4", " MIX5", " MIX6", " MIX7",
				" MIX8", " MIX9", " MIX10", " MIX11", " MIX12", " MIX13", " MIX14",
				" MIX15", " MIX16", " M/C", ""};

const char*   Xotpos[] = {"IN/LC", "IN/LC+M", "<-EQ", "<-EQ+M", "EQ->", "EQ->+M", "PRE", "PRE+M", "POST", ""};

const char*  XiQgrp[] = {" OFF", " A", " B", ""};
const char*  XiQspk[] = {" none", " iQ8", " iQ10", " iQ12", " iQ15", " iQ15B", " iQ18B", ""};
const char*   XiQeq[] = {" Linear", " Live", " Speech", " Playback", " User", ""};
const char* Psource[] = {" INT", " AES50A", " AES50B", " Exp Card", ""};
const char*  PSCont[] = {" CUES", " SCENES", " SNIPPETS", ""};
const char*   PRpro[] = {" MC", " HUI", " CC", ""};
const char*  PRrate[] = {" 48K", " 44K1", ""};
const char*   PRpos[] = {" PRE", " POST", ""};
const char*  PRmode[] = {" BAR", " SPEC", ""};
const char*   PRdet[] = {" RMS", " PEAK", ""};
const char*  PRport[] = {" MIDI", " CARD", " RTP", ""};
const char*  Pctype[] = {" FW", " USB", " unk", " unk", " unk", " unk", ""};
const char* Pusbmod[] = {" 32/32", " 16/16", " 32/8", " 8/32", " 8/8", " 2/2", ""};
const char* Pufmode[] = {" 32/32", " 16/16", " 32/8", " 8/32", ""};
const char*    Pcaw[] = {" IN", " OUT", ""};
const char*    Pcas[] = {" WC", " ADAT1", " ADAT2", " ADAT3", " ADAT4", ""};
const char* Pmdmode[] = {" 56", " 64", ""};
const char*  Pcmadi[] = {" 1-32", " 9-40", " 17-48", " 25-56", " 33-64", ""};
const char*  Pcmado[] = {" OFF", "1-32", " 9-40", " 17-48", " 25-56", " 33-64", ""};
const char* Pmadsrc[] = {" OFF", " OPT", " COAX", " BOTH", ""};
const char* Purectk[] = {" 32Ch", " 16Ch", " 8Ch", ""};
const char* Purplbk[] = {" SD", " USB", ""};
const char* Pursdsl[] = {" SD1", " SD2", ""};
const char* Purrctl[] = {" USB", " XLIVE", ""};
const char* Pinvmut[] = {" NORM", " INV", ""};
const char* Pclkmod[] = {" 24h", " 12h", ""};
const char* Purerpa[] = {" REC", " PLAY", " AUTO", ""};
const char* Prtavis[] = {" OFF", " 25%", " 30%", " 35%", " 40%", "45%", "50%", " 55%",
						" 60%", "65%", "70%", "75%", "80%", ""};
const char*  Prtaph[] = {" OFF", " 1", " 2", " 3", " 4", "5", "6", " 7", "8", ""};
const char*    Ubat[] = {" NONE", " GOOD", " LOW", ""};
const char*    Usdc[] = {" NONE", " READY", " PROTECT", " ERROR", ""};

const char* Sselidx[] = {" Ch01", " Ch02", " Ch03", " Ch04", " Ch05", " Ch06", " Ch07", " Ch08",
				" Ch09", " Ch10", " Ch11", " Ch12", " Ch13", " Ch14", " Ch15", " Ch16",
				" Ch17", " Ch18", " Ch19", " Ch20", " Ch21", " Ch22", " Ch23", " Ch24",
				" Ch25", " Ch26", " Ch27", " Ch28", " Ch29", " Ch30", " Ch31", " Ch32",
				" Aux1", " Aux2", " Aux3", " Aux4", " Aux5", " Aux6", " Aux7", " Aux8",
				" Fx1L", " Fx1R", " Fx2L", " Fx2R", " Fx3L", " Fx3R", " Fx4L", " Fx4R",
				" Bus1", " Bus2", " Bus3", " Bus4", " Bus5", " Bus6", " Bus7", " Bus8",
				" Bus9", " Bs10", " Bs11", " Bs12", " Bs13", " Bs14", " Bs15", " Bs16",
				" Mtx1", " Mtx2", " Mtx3", " Mtx4", " Mtx5", " Mtx6", " LR", " M/C", ""};
const char*	Sscrn[] = {" CHAN", " METERS", " ROUTE", " SETUP", " LIB", " FX",
				" MON", " USB", " SCENE", " ASSIGN", " LOCK", ""};
const char*	Schal[] = {" HOME", " CONFIG", " GATE", " DYN", " EQ", "MIX", " MAIN", ""};
const char*	Smetl[] = {" CHANNEL", " MIXBUS", " AUX/FX", " IN/OUT", " RTA", ""};
const char*	Sroul[] = {" HOME", " AES50A", " AES50B", " CARDOUT", "XLROUT", " ANAOUT", "AUXOUT", "P16OUT", "USER"};
const char*	Ssetl[] = {" GLOB", " CONF", " REMOTE", " NETW", "NAMES", "PREAMPS", " CARD", ""};
const char*	Slibl[] = {" CHAN", " EFFECT", " ROUTE", ""};
const char*	 Sfxl[] = {" HOME", " FX1", " FX2", " FX3", "FX4", "FX5", " FX6", " FX7", " FX8", ""};
const char*	Smonl[] = {" MONITOR", " TALKA", " TALKB", " OSC", ""};
const char*	Susbl[] = {" HOME", " CONFIG", ""};
const char*	Sscel[] = {" HOME", " SCENES", " BITS", " PARSAFE", "CHNSAFE", "MIDI", ""};
const char*	Sassl[] = {" HOME", " SETA", " SETB", " SETC", ""};
const char*	Stapl[] = {" STOP", " PPAUSE", " PLAY", " RPAUSE", "RECORD", "FF", "REW", ""};

const char* R00[] = {"OFF", "ON", ""};
const char* R01[] = {"FRONT", "REAR", ""};
const char* R02[] = {"ST", "M/S", ""};
const char* R03[] = {"2", "8", "12", "20", "ALL", ""};
const char* R04[] = {"COMP", "LIM", ""};
const char* R05[] = {"GR", "SBC", "PEAK", ""};
const char* R06[] = {"0", "1", ""};
const char* R07[] = {"OFF", "Bd1", "Bd2", "Bd3", "Bd4", "Bd5", ""};
const char* R08[] = {"12", "48", ""};
const char* R09[] = {"1.1", "1.2", "1.3", "1.5", "1.7", "2", "2.5", "3", "3.5", "4", "5", "7", "10", "LIM", ""};
const char* R10[] = {"1k5", "2k", "3k", "4k", "5k", ""};
const char* R11[] = {"200", "300", "500", "700", "1k", "1k5", "2k", "3k", "4k", "5k", "7k", ""};
const char* R12[] = {"200", "300", "500", "700", "1000", ""};
const char* R13[] = {"5k", "10k", "20k", ""};
const char* R14[] = {"3k", "4k", "5k", "8k", "10k", "12k", "16k", ""};
const char* R15[] = {"20", "30", "60", "100", ""};
const char* R16[] = {"FEM", "MALE", ""};
const char* R17[] = {"AMB", "CLUB", "HALL", ""};
const char* R18[] = {"PAR", "SER", ""};
const char* R19[] = {"1", "1/2", "2/3", "3/2", ""};
const char* R20[] = {"1/4", "1/3", "3/8", "1/2", "2/3", "3/4", "1", "1/4X", "1/3X", "3/8X", "1/2X", "2/3X", "3/4X", "1X",""};
const char* R21[] = {"LO", "MID", "HI",""};
const char* R22[] = {"TRI", "SIN", "SAW", "SAW-", "RMP", "SQU", "RND",""};
const char* R23[] = {"LP", "HP", "BP", "NO",""};
const char* R24[] = {"M", "ST",""};
const char* R25[] = {"1/4", "3/8", "1/2", "2/3", "1", "4/3", "3/2", "2", "3",""};
const char* R26[] = {"2POL", "4POL",""};
const char* R27[] = {"RUN", "STOP",""};
const char* R28[] = {"SLOW", "FAST",""};
const char* R29[] = {"ST", "MS",""};


union littlebig {	//
	int ii;			// A small union to manage
	float ff;		// Endian type conversions
	char cc[4];		//
} endian;

enum types {
	NIL,		// 0
	I32,		// 1
	F32,		// 2
	S32,		// 3
	B32,		// 4
	E32,		// 5
	P32,		// 6
	RES2,		// 7
	RES3,		// 8
	RES4,		// 9
	FX32,		// 10
	OFFON,		// 11
	CMONO,		// 12
	CSOLO,		// 13
	CTALK,		// 14
	CTALKAB,	// 15
	COSC,		// 16
	CROUTSW,	// 17
	CROUTIN,	// 18
	CROUTAC,	// 19
	CROUTOT,	// 20
	CROUTPLAY,	// 21
	CCTRL,		// 22
	CENC,		// 23
	CTAPE,		// 24
	CMIX,		// 25
	CHCO,		// 26
	CHDE,		// 27
	CHPR,		// 28
	CHGA,		// 29
	CHGF,		// 30
	CHDY,		// 31
	CHDF,		// 32
	CHIN,		// 33
	CHEQ,		// 34
	CHMX,		// 35
	CHMO,		// 36
	CHME,		// 37
	CHGRP,		// 38
	CHAMIX,		// 39
	AXPR,		// 40
	BSCO,		// 41
	MXPR,		// 42
	MXDY,		// 43
	MSMX,		// 44
	FXTYP1,		// 45
	FXSRC,		// 46
	FXPAR1,		// 47
	FXTYP2,		// 48
	FXPAR2,		// 49
	OMAIN,		// 50
	OMAIN2,		// 51
	OP16,		// 52
	OMAIND,		// 53
	HAMP,		// 54
	PREFS,		// 55
	PIR,		// 56
	PIQ,		// 57
	PCARD,		// 58
	PRTA,		// 59
	PIP,		// 60
	PADDR,		// 61
	PMASK,		// 62
	PGWAY,		// 63
	STAT,		// 64
	SSCREEN,	// 65
	SCHA,		// 66
	SMET,		// 67
	SROU,		// 68
	SSET,		// 69
	SLIB,		// 70
	SFX,		// 71
	SMON,		// 72
	SUSB,		// 73
	SSCE,		// 74
	SASS,		// 75
	SSOLOSW,	// 76
	SAES,		// 77
	STAPE,		// 78
	SOSC,		// 79
	STALK,		// 80
	USB,		// 81
	SNAM,		// 82
	SCUE,		// 83
	SSCN,		// 84
	SSNP,		// 85
	HA,			// 86
	ACTION,		// 87
	UREC,		// 88
	SLIBS,		// 89
	D48,		// 90
	D48A,		// 91
	D48G,		// 92
	UROUO,		// 93
	UROUI,		// 94
	PKEY,		// 95
};

typedef struct X32command {	// The Command structure describes ALL X32 commands
	char* command;			// - Their syntax (/ch/01/config/icon (for example))
	union {					// - Their format type (no data, int, float, string, blob, special type...)
		int typ;			// see enum types above
		char* str;			// address of a string if applicable
	} format;				//
	int flags;				// - a command flag: (simple command, get/set, node entry, etc.)
	union {					// - The value associated to the format type above
		int ii;				// can be int, float, string address or general data address
		float ff;			//
		char* str;			//
		void* dta;			//
	} value;				//
	const char* *node;			// when applicable, the array of node strings associated with the command
							// the actual node value is node[value.ii]. This pointer is NULL if the data
							// should be directly obtained from value.type and printed as a string.
} X32command;				//

typedef struct X32enum {	// X32 enum structure
	char*	str;			// X32 enum name
	int		ival;			// X32 corresponding int value
} X32enum;

typedef struct X32node {	// The Node header structure is used to directly
	char* command;			// parse a node command on a limited number of characters and
	int nchars;				// "jump" to the associated node entry function to manage
	X32command* cmd_ptr;	// the appropriate "node ,s ..." answer
	int cmd_max;			//
} X32node;

struct {					// The Client structure
	int vlid;				// - Valid entry
	struct sockaddr sock;	// - Client identification data (based on IP/Port)
	time_t xrem;			// - /xremote time when initiated
} X32Client[MAX_CLIENTS];

char snode_str[32]; // used to temporarily save a string in node and FX commands

typedef struct X32header {	// The Header structure is used to quickly scan through
	union {					// blocks of commands (/config, /ch/xxx, etc) with
		char ccom[5];		// a single test on the first four characters.
		int icom;			//
	} header;				// A union enables doing this on a single int test
	int (*fptr)();			// The result points to a parsing function, via a
} X32header;