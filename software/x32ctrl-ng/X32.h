#pragma once

#ifdef __WIN32__
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#define ZMemory(a,b)	ZeroMemory(a, b)
#else
#define ZMemory(a,b)	memset(a, 0, b)
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SOCKET_ERROR -1
#define WSAGetLastError() -1
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include "constants.h"

#define EPSILON 0.0001	// epsilon for float comparisons
#define XVERSION "4.06"	// FW version
#define BSIZE 512		// Buffer sizes
#define X32DEBUG 0		// default debug mode
#define X32VERBOSE 1	// default verbose mode
#define VERB_REMOTE 0	// if verbose: default /xremote verbose
#define VERB_BATCH 0	// if verbose: default /batch... verbose
#define VERB_FORMAT 0	// if verbose: default /format... verbose
#define VERB_RENEW 0	// if verbose: default /renew verbose
#define VERB_METER 0	// if verbose: default /meters verbose
#define MAX_CLIENTS 4	// support updating 4 remote clients max
#define MAX_METERS 17	// support for max /meters, starting with /meters/0
#define XREMOTE_TIME 11	// xremote max time before abandon of client updating

#define F_GET 0x0001	// Get parameter(s) command
#define F_SET 0x0002	// Set parameter(s) command
#define F_XET F_GET | F_SET
#define F_NPR 0x0004	// Do not propagate to other clients
#define F_FND 0x0008	// first of a series or node data header

#define S_SND 0x0001	// Update current client needed
#define S_REM 0x0002	// Update remote clients needed

#define C_HA 0x0001		// copy mask : Head amps
#define C_CONFIG 0x0002	// copy mask : Config
#define C_GATE 0x0004	// copy mask : Gate
#define C_DYN 0x0008	// copy mask : Dynamics
#define C_EQ 0x0010		// copy mask : EQs
#define C_SEND 0x0020	// copy mask : Sends

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

typedef struct X32header {	// The Header structure is used to quickly scan through
	union {					// blocks of commands (/config, /ch/xxx, etc) with
		char ccom[4];		// a single test on the first four characters.
		int icom;			//
	} header;				// A union enables doing this on a single int test
	int (*fptr)();			// The result points to a parsing function, via a
} X32header;				// dedicated function pointer call

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
	char* *node;			// when applicable, the array of node strings associated with the command
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


//
// Private functions
void Xsend(int who_to);
void X32Print(struct X32command* command);
int X32Shutdown();
int X32Init();
int FXc_lookup(X32command* command, int i);
int funct_params(X32command* command, int i);
int function_shutdown();
int function_info();
int function_xinfo();
int function_status();
int function_xremote();
int function_slash();
int function_node();
int function_config();
int function_main();
int function_prefs();
int function_stat();
int function_urec();
int function_channel();
int function_auxin();
int function_fxrtn();
int function_bus();
int function_mtx();
int function_dca();
int function_fx();
int function_output();
int function_headamp();
int function_misc();
int function_show();
int function_copy();
int function_add();
int function_load();
int function_save();
int function_delete();
int function_unsubscribe();
int function_action();
int function_meters();
int function_misc();
int function_renew();
int function_libs();
int function_showdump();
int function();
//
float Xr_float(char* Xin, int l);

char* XslashSetInt(X32command* command, char* str_pt_in);
char* XslashSetPerInt(X32command* command, char* str_pt_in);
char* XslashSetString(X32command* command, char* str_pt_in);
char* XslashSetList(X32command* command, char* str_pt_in);
char* XslashSetLinf(X32command* command, char* str_pt_in, float f1, float f2, float step);
char* XslashSetLogf(X32command* command, char* str_pt_in, float f1, float f2, int steps);
char* XslashSetLevl(X32command* command, char* str_pt_in, int steps);

char* RLinf(X32command* command, char* str_pt_in, float xmin, float lmaxmin);
char* RLogf(X32command* command, char* str_pt_in, float xmin, float lmaxmin);
char* REnum(X32command* command, char* str_pt_in, char* str_enum[]);

int function_node_single();
void GetFxPar1(X32command* command, char* buf, int ipar, int type);
void SetFxPar1(X32command* command, char* str_pt_in, int ipar, int type);
void Xprepmeter(int i, int l, char *buf, int n, int k);



void Xdump(char *buf, int len, int debug);
void Xfdump(char *header, char *buf, int len, int debug);
void Xsdump(char *str_out, char *buf, int len);

Xsprint(char *bd, int index, char format, void *bs);
Xfprint(char *bd, int index, char* text, char format, void *bs);



char snode_str[32]; // used to temporarily save a string in node and FX commands
char*	OffOn[] = {" OFF", " ON", ""};
char* Xamxgrp[] = {" OFF", " X", " Y", ""};
char* Xcolors[] = {" OFF", " RD", " GN", " YE", " BL", " MG", " CY", " WH", " OFFi", " RDi", " GNi", " YEi", " BLi", " MGi", " CYi", " WHi", ""};
char* XSsourc[] = {" OFF", " LR", " LR+C", " LRPFL", " LRAFL", " AUX56", " AUX78", ""};
char* Xmnmode[] = {" LR+M", " LCR", ""};
char* Xchmode[] = {" PFL", " AFL", ""};
char*  Xhslop[] = {" 12", " 18", " 24", ""};
char*  Xgmode[] = {" EXP2", " EXP3", " EXP", " GATE", " DUCK", ""};
char* Xdymode[] = {" COMP", " EXP", ""};
char*  Xdydet[] = {" PEAK", " RMS", ""};
char*  Xdyenv[] = {" LIN", " LOG", ""};
char*  Xdyrat[] = {" 1.1", " 1.3", " 1.5", " 2.0", " 2.5", " 3.0", " 4.0", " 5.0", " 7.0", " 10", " 20", " 100", ""};
char* Xdyftyp[] = {" LC6", " LC12", " HC6", " HC12", " 1.0", " 2.0", " 3.0", " 5.0", " 10.0", ""};
char* Xdyppos[] = {" PRE", " POST", ""};
char*   Xisel[] = {" OFF", " FX1L", " FX1R", " FX2L", " FX2R", " FX3L", " FX3R",
                   " FX4L", " FX4R", " FX5L", " FX5R", " FX6L", " FX6R", " FX7L",
                   " FX7R", " FX8L", " FX8R", " AUX1", " AUX2", " AUX3", " AUX4",
                   " AUX5", " AUX6", ""};
char*  Xeqty1[] = {" LCut", " LShv", " PEQ", " VEQ", " HShv", " HCut", ""};
char*  Xeqty2[] = {" LCut", " LShv", " PEQ", " VEQ", " HShv", " HCut", " BU6",
                   " BU12", " BS12", " LR12", " BU18", " BU24", " BS24", " LR24", ""};
char*  Xmtype[] = {" IN/LC", " <-EQ", " EQ->", " PRE", " POST", " GRP", ""};
char* XTsourc[] = {" INT", " EXT", ""};
char* XOscsel[] = {" F1", " F2", ""};
char* XOsctyp[] = {" SINE", " PINK", " WHITE", ""};
char*  XCFrsw[] = {" REC", " PLAY", ""};
char*  XRtgin[] = {" AN1-8", " AN9-16", " AN17-24", " AN25-32", " A1-8", " A9-16", " A17-24", " A25-32", " A33-40",
                   " A41-48", " B1-8", " B9-16", " B17-24",	" B25-32", " B33-40", " B41-48", " CARD1-8", " CARD9-16",
                   " CARD17-24", " CARD25-32", ""};
char*  XRtaea[] = {" AN1-8", " AN9-16", "AN17-24", " AN25-32", " A1-8",	" A9-16", " A17-24", " A25-32", " A33-40",
                   " A41-48", " B1-8", " B9-16", " B17-24",	" B25-32", " B33-40", " B41-48", " CARD1-8", " CARD9-16",
                   " CARD17-24", " CARD25-32", " OUT1-8", " OUT9-16", " P161-8", " P169-16", " AUX1-6/Mon", " AuxIN1-6/TB", ""};
char*  XRtina[] = {" AUX1-4", " AN1-2", " AN1-4", " AN1-6", " A1-2", " A1-4", " A1-6",
                   " B1-2", " B1-4", " B1-6", " CARD1-2", " CARD1-4", " CARD1-6", ""};
char*  XRout1[] = {" AN1-4", " AN9-12", " AN17-20", " AN25-28", " A1-4", " A9-12", " A17-20", " A25-28", " A33-36",
                   " A41-44" ," B1-4", " B9-12", " B17-20", " B25-28", " B33-36", " B41-44", " CARD1-4", " CARD9-12",
                   " CARD17-20", " CARD25-28", " OUT1-4", " OUT9-12", " P161-4", " P169-12", " AUX/CR", " AUX/TB", ""};
char*  XRout5[] = {" AN5-8", " AN13-16", " AN21-24", " AN29-32", " A5-8", " A13-16", " A21-24", " A29-32", " A37-40",
                   " A45-48" ," B5-8", " B13-16", " B21-24", " B29-32", " B37-40", " B45-48", " CARD5-8", " CARD13-16",
                   " CARD21-24", " CARD29-32", " OUT5-8", " OUT13-16", " P165-8", " P1613-16", " AUX/CR", " AUX/TB", ""};

char* Sfxtyp1[] = {" HALL", " AMBI", " RPLT", " ROOM", " CHAM", " PLAT", " VREV", " VRM",
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

char* Sfxtyp2[] = {" GEQ2", " TEQ2", " GEQ", " TEQ", " DES2", " DES", " P1A", " P1A2" ,
                   " PQ5", " PQ5S", " WAVD", " LIM", " FAC", " FAC1M", " FAC2", " LEC", " LEC2", " ULC" ,
                   " ULC2", " ENH2", " ENH", " EXC2", " EXC", " IMG", " EDI", " SON", " AMP2", " AMP" ,
                   " DRV2", " DRV", " PHAS", " FILT", " PAN", " SUB", ""};

enum Sfxtyp2 {_2_GEQ2 = _1_PIT + 2, _2_TEQ2, _2_GEQ, _2_TEQ, _2_DES2, _2_DES, _2_P1A, _2_P1A2,
              _2_PQ5S, _2_PQ5, _2_WAVD, _2_LIM, _2_FAC2, _2_FAC1M, _2_FAC, _2_LEC2, _2_LEC, _2_ULC2,
              _2_ULC, _2_ENH2, _2_ENH, _2_EXC2, _2_EXC, _2_IMG, _2_EDI, _2_SON, _2_AMP2, _2_AMP,
              _2_DRV2, _2_DRV, _2_PHAS, _2_FILT, _2_PAN, _2_SUB};

char*  Sfxsrc[] = {" INS", " MIX1", " MIX2", " MIX3", " MIX4", " MIX5", " MIX6", " MIX7",
                  " MIX8", " MIX9", " MIX10", " MIX11", " MIX12", " MIX13", " MIX14",
                  " MIX15", " MIX16", " M/C", ""};

char*   Xotpos[] = {"IN/LC", "IN/LC+M", "<-EQ", "<-EQ+M", "EQ->", "EQ->+M", "PRE", "PRE+M", "POST", ""};

char*  XiQgrp[] = {" OFF", " A", " B", ""};
char*  XiQspk[] = {" none", " iQ8", " iQ10", " iQ12", " iQ15", " iQ15B", " iQ18B", ""};
char*   XiQeq[] = {" Linear", " Live", " Speech", " Playback", " User", ""};
char* Psource[] = {" INT", " AES50A", " AES50B", " Exp Card", ""};
char*  PSCont[] = {" CUES", " SCENES", " SNIPPETS", ""};
char*   PRpro[] = {" MC", " HUI", " CC", ""};
char*  PRrate[] = {" 48K", " 44K1", ""};
char*   PRpos[] = {" PRE", " POST", ""};
char*  PRmode[] = {" BAR", " SPEC", ""};
char*   PRdet[] = {" RMS", " PEAK", ""};
char*  PRport[] = {" MIDI", " CARD", " RTP", ""};
char*  Pctype[] = {" FW", " USB", " unk", " unk", " unk", " unk", ""};
char* Pusbmod[] = {" 32/32", " 16/16", " 32/8", " 8/32", " 8/8", " 2/2", ""};
char* Pufmode[] = {" 32/32", " 16/16", " 32/8", " 8/32", ""};
char*    Pcaw[] = {" IN", " OUT", ""};
char*    Pcas[] = {" WC", " ADAT1", " ADAT2", " ADAT3", " ADAT4", ""};
char* Pmdmode[] = {" 56", " 64", ""};
char*  Pcmadi[] = {" 1-32", " 9-40", " 17-48", " 25-56", " 33-64", ""};
char*  Pcmado[] = {" OFF", "1-32", " 9-40", " 17-48", " 25-56", " 33-64", ""};
char* Pmadsrc[] = {" OFF", " OPT", " COAX", " BOTH", ""};
char* Purectk[] = {" 32Ch", " 16Ch", " 8Ch", ""};
char* Purplbk[] = {" SD", " USB", ""};
char* Pursdsl[] = {" SD1", " SD2", ""};
char* Purrctl[] = {" USB", " XLIVE", ""};
char* Pinvmut[] = {" NORM", " INV", ""};
char* Pclkmod[] = {" 24h", " 12h", ""};
char* Purerpa[] = {" REC", " PLAY", " AUTO", ""};
char* Prtavis[] = {" OFF", " 25%", " 30%", " 35%", " 40%", "45%", "50%", " 55%",
						 " 60%", "65%", "70%", "75%", "80%", ""};
char*  Prtaph[] = {" OFF", " 1", " 2", " 3", " 4", "5", "6", " 7", "8", ""};
char*    Ubat[] = {" NONE", " GOOD", " LOW", ""};
char*    Usdc[] = {" NONE", " READY", " PROTECT", " ERROR", ""};

char* Sselidx[] = {" Ch01", " Ch02", " Ch03", " Ch04", " Ch05", " Ch06", " Ch07", " Ch08",
                   " Ch09", " Ch10", " Ch11", " Ch12", " Ch13", " Ch14", " Ch15", " Ch16",
                   " Ch17", " Ch18", " Ch19", " Ch20", " Ch21", " Ch22", " Ch23", " Ch24",
                   " Ch25", " Ch26", " Ch27", " Ch28", " Ch29", " Ch30", " Ch31", " Ch32",
                   " Aux1", " Aux2", " Aux3", " Aux4", " Aux5", " Aux6", " Aux7", " Aux8",
                   " Fx1L", " Fx1R", " Fx2L", " Fx2R", " Fx3L", " Fx3R", " Fx4L", " Fx4R",
                   " Bus1", " Bus2", " Bus3", " Bus4", " Bus5", " Bus6", " Bus7", " Bus8",
                   " Bus9", " Bs10", " Bs11", " Bs12", " Bs13", " Bs14", " Bs15", " Bs16",
                   " Mtx1", " Mtx2", " Mtx3", " Mtx4", " Mtx5", " Mtx6", " LR", " M/C", ""};
char*	Sscrn[] = {" CHAN", " METERS", " ROUTE", " SETUP", " LIB", " FX",
				   " MON", " USB", " SCENE", " ASSIGN", " LOCK", ""};
char*	Schal[] = {" HOME", " CONFIG", " GATE", " DYN", " EQ", "MIX", " MAIN", ""};
char*	Smetl[] = {" CHANNEL", " MIXBUS", " AUX/FX", " IN/OUT", " RTA", ""};
char*	Sroul[] = {" HOME", " AES50A", " AES50B", " CARDOUT", "XLROUT", " ANAOUT", "AUXOUT", "P16OUT", "USER"};
char*	Ssetl[] = {" GLOB", " CONF", " REMOTE", " NETW", "NAMES", "PREAMPS", " CARD", ""};
char*	Slibl[] = {" CHAN", " EFFECT", " ROUTE", ""};
char*	 Sfxl[] = {" HOME", " FX1", " FX2", " FX3", "FX4", "FX5", " FX6", " FX7", " FX8", ""};
char*	Smonl[] = {" MONITOR", " TALKA", " TALKB", " OSC", ""};
char*	Susbl[] = {" HOME", " CONFIG", ""};
char*	Sscel[] = {" HOME", " SCENES", " BITS", " PARSAFE", "CHNSAFE", "MIDI", ""};
char*	Sassl[] = {" HOME", " SETA", " SETB", " SETC", ""};
char*	Stapl[] = {" STOP", " PPAUSE", " PLAY", " RPAUSE", "RECORD", "FF", "REW", ""};

char* R00[] = {"OFF", "ON", ""};
char* R01[] = {"FRONT", "REAR", ""};
char* R02[] = {"ST", "M/S", ""};
char* R03[] = {"2", "8", "12", "20", "ALL", ""};
char* R04[] = {"COMP", "LIM", ""};
char* R05[] = {"GR", "SBC", "PEAK", ""};
char* R06[] = {"0", "1", ""};
char* R07[] = {"OFF", "Bd1", "Bd2", "Bd3", "Bd4", "Bd5", ""};
char* R08[] = {"12", "48", ""};
char* R09[] = {"1.1", "1.2", "1.3", "1.5", "1.7", "2", "2.5", "3", "3.5", "4", "5", "7", "10", "LIM", ""};
char* R10[] = {"1k5", "2k", "3k", "4k", "5k", ""};
char* R11[] = {"200", "300", "500", "700", "1k", "1k5", "2k", "3k", "4k", "5k", "7k", ""};
char* R12[] = {"200", "300", "500", "700", "1000", ""};
char* R13[] = {"5k", "10k", "20k", ""};
char* R14[] = {"3k", "4k", "5k", "8k", "10k", "12k", "16k", ""};
char* R15[] = {"20", "30", "60", "100", ""};
char* R16[] = {"FEM", "MALE", ""};
char* R17[] = {"AMB", "CLUB", "HALL", ""};
char* R18[] = {"PAR", "SER", ""};
char* R19[] = {"1", "1/2", "2/3", "3/2", ""};
char* R20[] = {"1/4", "1/3", "3/8", "1/2", "2/3", "3/4", "1", "1/4X", "1/3X", "3/8X", "1/2X", "2/3X", "3/4X", "1X",""};
char* R21[] = {"LO", "MID", "HI",""};
char* R22[] = {"TRI", "SIN", "SAW", "SAW-", "RMP", "SQU", "RND",""};
char* R23[] = {"LP", "HP", "BP", "NO",""};
char* R24[] = {"M", "ST",""};
char* R25[] = {"1/4", "3/8", "1/2", "2/3", "1", "4/3", "3/2", "2", "3",""};
char* R26[] = {"2POL", "4POL",""};
char* R27[] = {"RUN", "STOP",""};
char* R28[] = {"SLOW", "FAST",""};
char* R29[] = {"ST", "MS",""};

#include "X32Channel.h"		//
#include "X32CfgMain.h"		//
#include "X32PrefStat.h"	// These are the actual files describing
#include "X32Auxin.h"		// X32 Commands by blocks of the same
#include "X32Fxrtn.h"		// type of commands... More than 10k commands!
#include "X32Bus.h"			//
#include "X32Mtx.h"			//
#include "X32Dca.h"			//
#include "X32Fx.h"			//
#include "X32Output.h"		//
#include "X32Headamp.h"		//
#include "X32Show.h"		//
#include "X32Misc.h"		//
#include "X32Libs.h"		//


X32header Xheader[] = { // X32 Headers, the data used for testing and the
	{ { "/shu" }, &function_shutdown }, // associated function call
	{ { "/inf" }, &function_info },
	{ { "/xin" }, &function_xinfo },
	{ { "/sta" }, &function_status },
	{ { "/xre" }, &function_xremote },
	{ { "/nod" }, &function_node },
	{ {"/\0\0\0" }, &function_slash },
	{ { "/con" }, &function_config },
	{ { "/mai" }, &function_main },
	{ { "/-pr" }, &function_prefs },
	{ { "/-st" }, &function_stat },
	{ { "/-ur" }, &function_urec },
	{ { "/ch/" }, &function_channel },
	{ { "/aux" }, &function_auxin },
	{ { "/fxr" }, &function_fxrtn },
	{ { "/bus" }, &function_bus },
	{ { "/mtx" }, &function_mtx },
	{ { "/dca" }, &function_dca },
	{ { "/fx/" }, &function_fx },
	{ { "/out" }, &function_output },
	{ { "/hea" }, &function_headamp },
	{ { "/met" }, &function_meters },
	{ { "/-ha" }, &function_misc },
	{ { "/ins" }, &function_misc },
	{ { "/-sh" }, &function_show },
	{ { "/ren" }, &function_renew },
	{ { "/cop" }, &function_copy },
	{ { "/add" }, &function_add },
	{ { "/loa" }, &function_load },
	{ { "/sav" }, &function_save },
	{ { "/del" }, &function_delete },
	{ { "/uns" }, &function_unsubscribe },
	{ { "/-us" }, &function_misc },
	{ { "/und" }, &function },
	{ { "/-ac" }, &function_action },
	{ { "/-li" }, &function_libs },
	{ { "/sho" }, &function_showdump },
};
int Xheader_max = sizeof(Xheader) / sizeof(X32header);

X32command Xmeters[] = {
		{"/meters/0",		{I32}, F_GET, {0}, NULL},			// 0
		{"/meters/1",		{I32}, F_GET, {0}, NULL},			// 1
		{"/meters/2",		{I32}, F_GET, {0}, NULL},			// 2
		{"/meters/3",		{I32}, F_GET, {0}, NULL},			// 3
		{"/meters/4",		{I32}, F_GET, {0}, NULL},			// 4
		{"/meters/5",		{I32}, F_GET, {0}, NULL},			// 5
		{"/meters/6",		{I32}, F_GET, {0}, NULL},			// 6
		{"/meters/7",		{I32}, F_GET, {0}, NULL},			// 7
		{"/meters/8",		{I32}, F_GET, {0}, NULL},			// 8
		{"/meters/9",		{I32}, F_GET, {0}, NULL},			// 9
		{"/meters/10",		{I32}, F_GET, {0}, NULL},			// 10
		{"/meters/11",		{I32}, F_GET, {0}, NULL},			// 11
		{"/meters/12",		{I32}, F_GET, {0}, NULL},			// 12
		{"/meters/13",		{I32}, F_GET, {0}, NULL},			// 13
		{"/meters/14",		{I32}, F_GET, {0}, NULL},			// 14
		{"/meters/15",		{I32}, F_GET, {0}, NULL},			// 15
		{"/meters/16",		{I32}, F_GET, {0}, NULL},			// 16

};
int Xmeters_max = sizeof(Xmeters) / sizeof(X32command);
//
// Active meters (meters/0...meters/15) Time when to stop (Active) and interval between 2 consecutive meters (Inter)
struct timeval		xmeter_time;
struct timeval 		XTimerMeters[MAX_METERS];
struct timeval 		XInterMeters[MAX_METERS];
long           		XDeltaMeters[MAX_METERS];
struct sockaddr 	XClientMeters[MAX_METERS];
char				Xbuf_meters[MAX_METERS][512];
int					Lbuf_meters[MAX_METERS];
int					XActiveMeters;
//
#ifndef timerincrement
#define	timerincrement(a, b)								\
	do {													\
		(a)->tv_usec += (b);								\
		while ((a)->tv_usec >= 1000000) {					\
			++(a)->tv_sec;									\
			(a)->tv_usec -= 1000000;						\
		}													\
	} while (0)
#endif
//
#ifndef timercmp
#define timercmp(a, b, CMP)									\
	do {													\
		  (((a)->tv_sec == (b)->tv_sec) ?					\
		   ((a)->tv_usec CMP (b)->tv_usec) :				\
		   ((a)->tv_sec CMP (b)->tv_sec))					\
	} while (0)
#endif
//


X32command Xdummy[] = { };
int Xdummy_max = sizeof(Xdummy) / sizeof(X32command);

X32node Xnode[] = { // /node Command Headers (see structure definition above
	{ "conf", 4, Xconfig, sizeof(Xconfig)/sizeof(X32command)},
	{ "main", 4, Xmain, sizeof(Xmain)/sizeof(X32command)},
	{ "-pre", 4, Xprefs, sizeof(Xprefs)/sizeof(X32command)},
	{ "-sta", 4, Xstat, sizeof(Xstat)/sizeof(X32command)},
	{ "ch/01", 5, Xchannel01, sizeof(Xchannel01)/sizeof(X32command)},
	{ "ch/02", 5, Xchannel02, sizeof(Xchannel02)/sizeof(X32command)},
	{ "ch/03", 5, Xchannel03, sizeof(Xchannel03)/sizeof(X32command)},
	{ "ch/04", 5, Xchannel04, sizeof(Xchannel04)/sizeof(X32command)},
	{ "ch/05", 5, Xchannel05, sizeof(Xchannel05)/sizeof(X32command)},
	{ "ch/06", 5, Xchannel06, sizeof(Xchannel06)/sizeof(X32command)},
	{ "ch/07", 5, Xchannel07, sizeof(Xchannel07)/sizeof(X32command)},
	{ "ch/08", 5, Xchannel08, sizeof(Xchannel08)/sizeof(X32command)},
	{ "ch/09", 5, Xchannel09, sizeof(Xchannel09)/sizeof(X32command)},
	{ "ch/10", 5, Xchannel10, sizeof(Xchannel10)/sizeof(X32command)},
	{ "ch/11", 5, Xchannel11, sizeof(Xchannel11)/sizeof(X32command)},
	{ "ch/12", 5, Xchannel12, sizeof(Xchannel12)/sizeof(X32command)},
	{ "ch/13", 5, Xchannel13, sizeof(Xchannel13)/sizeof(X32command)},
	{ "ch/14", 5, Xchannel14, sizeof(Xchannel14)/sizeof(X32command)},
	{ "ch/15", 5, Xchannel15, sizeof(Xchannel15)/sizeof(X32command)},
	{ "ch/16", 5, Xchannel16, sizeof(Xchannel16)/sizeof(X32command)},
	{ "ch/17", 5, Xchannel17, sizeof(Xchannel17)/sizeof(X32command)},
	{ "ch/18", 5, Xchannel18, sizeof(Xchannel18)/sizeof(X32command)},
	{ "ch/19", 5, Xchannel19, sizeof(Xchannel19)/sizeof(X32command)},
	{ "ch/20", 5, Xchannel20, sizeof(Xchannel20)/sizeof(X32command)},
	{ "ch/21", 5, Xchannel21, sizeof(Xchannel21)/sizeof(X32command)},
	{ "ch/22", 5, Xchannel22, sizeof(Xchannel22)/sizeof(X32command)},
	{ "ch/23", 5, Xchannel23, sizeof(Xchannel23)/sizeof(X32command)},
	{ "ch/24", 5, Xchannel24, sizeof(Xchannel24)/sizeof(X32command)},
	{ "ch/25", 5, Xchannel25, sizeof(Xchannel25)/sizeof(X32command)},
	{ "ch/26", 5, Xchannel26, sizeof(Xchannel26)/sizeof(X32command)},
	{ "ch/27", 5, Xchannel27, sizeof(Xchannel27)/sizeof(X32command)},
	{ "ch/28", 5, Xchannel28, sizeof(Xchannel28)/sizeof(X32command)},
	{ "ch/29", 5, Xchannel29, sizeof(Xchannel29)/sizeof(X32command)},
	{ "ch/30", 5, Xchannel30, sizeof(Xchannel30)/sizeof(X32command)},
	{ "ch/31", 5, Xchannel31, sizeof(Xchannel31)/sizeof(X32command)},
	{ "ch/32", 5, Xchannel32, sizeof(Xchannel32)/sizeof(X32command)},
	{ "ch", 2, Xchannel01, sizeof(Xchannel01)/sizeof(X32command)},
	{ "auxin/01", 8, Xauxin01, sizeof(Xauxin01)/sizeof(X32command)},
	{ "auxin/02", 8, Xauxin02, sizeof(Xauxin02)/sizeof(X32command)},
	{ "auxin/03", 8, Xauxin03, sizeof(Xauxin03)/sizeof(X32command)},
	{ "auxin/04", 8, Xauxin04, sizeof(Xauxin04)/sizeof(X32command)},
	{ "auxin/05", 8, Xauxin05, sizeof(Xauxin05)/sizeof(X32command)},
	{ "auxin/06", 8, Xauxin06, sizeof(Xauxin06)/sizeof(X32command)},
	{ "auxin/07", 8, Xauxin07, sizeof(Xauxin07)/sizeof(X32command)},
	{ "auxin/08", 8, Xauxin08, sizeof(Xauxin08)/sizeof(X32command)},
	{ "auxin", 5, Xauxin01, sizeof(Xauxin01)/sizeof(X32command)},
	{ "fxrtn/01", 8, Xfxrtn01, sizeof(Xfxrtn01)/sizeof(X32command)},
	{ "fxrtn/02", 8, Xfxrtn02, sizeof(Xfxrtn02)/sizeof(X32command)},
	{ "fxrtn/03", 8, Xfxrtn03, sizeof(Xfxrtn03)/sizeof(X32command)},
	{ "fxrtn/04", 8, Xfxrtn04, sizeof(Xfxrtn04)/sizeof(X32command)},
	{ "fxrtn/05", 8, Xfxrtn05, sizeof(Xfxrtn05)/sizeof(X32command)},
	{ "fxrtn/06", 8, Xfxrtn06, sizeof(Xfxrtn06)/sizeof(X32command)},
	{ "fxrtn/07", 8, Xfxrtn07, sizeof(Xfxrtn07)/sizeof(X32command)},
	{ "fxrtn/08", 8, Xfxrtn08, sizeof(Xfxrtn08)/sizeof(X32command)},
	{ "fxrtn", 5, Xfxrtn01, sizeof(Xfxrtn01)/sizeof(X32command)},
	{ "fx/1", 4, Xfx1, sizeof(Xfx1)/sizeof(X32command)},
	{ "fx/2", 4, Xfx2, sizeof(Xfx2)/sizeof(X32command)},
	{ "fx/3", 4, Xfx3, sizeof(Xfx3)/sizeof(X32command)},
	{ "fx/4", 4, Xfx4, sizeof(Xfx4)/sizeof(X32command)},
	{ "fx/5", 4, Xfx5, sizeof(Xfx5)/sizeof(X32command)},
	{ "fx/6", 4, Xfx6, sizeof(Xfx6)/sizeof(X32command)},
	{ "fx/7", 4, Xfx7, sizeof(Xfx7)/sizeof(X32command)},
	{ "fx/8", 4, Xfx8, sizeof(Xfx8)/sizeof(X32command)},
	{ "fx", 2, Xfx1, sizeof(Xfx1)/sizeof(X32command)},
	{ "bus/01", 6, Xbus01, sizeof(Xbus01)/sizeof(X32command)},
	{ "bus/02", 6, Xbus02, sizeof(Xbus02)/sizeof(X32command)},
	{ "bus/03", 6, Xbus03, sizeof(Xbus03)/sizeof(X32command)},
	{ "bus/04", 6, Xbus04, sizeof(Xbus04)/sizeof(X32command)},
	{ "bus/05", 6, Xbus05, sizeof(Xbus05)/sizeof(X32command)},
	{ "bus/06", 6, Xbus06, sizeof(Xbus06)/sizeof(X32command)},
	{ "bus/07", 6, Xbus07, sizeof(Xbus07)/sizeof(X32command)},
	{ "bus/08", 6, Xbus08, sizeof(Xbus08)/sizeof(X32command)},
	{ "bus/09", 6, Xbus09, sizeof(Xbus09)/sizeof(X32command)},
	{ "bus/10", 6, Xbus10, sizeof(Xbus10)/sizeof(X32command)},
	{ "bus/11", 6, Xbus11, sizeof(Xbus11)/sizeof(X32command)},
	{ "bus/12", 6, Xbus12, sizeof(Xbus12)/sizeof(X32command)},
	{ "bus/13", 6, Xbus13, sizeof(Xbus13)/sizeof(X32command)},
	{ "bus/14", 6, Xbus14, sizeof(Xbus14)/sizeof(X32command)},
	{ "bus/15", 6, Xbus15, sizeof(Xbus15)/sizeof(X32command)},
	{ "bus/16", 6, Xbus16, sizeof(Xbus16)/sizeof(X32command)},
	{ "bus", 3, Xbus01, sizeof(Xbus01)/sizeof(X32command)},
	{ "mtx/01", 6, Xmtx01, sizeof(Xmtx01)/sizeof(X32command)},
	{ "mtx/02", 6, Xmtx02, sizeof(Xmtx02)/sizeof(X32command)},
	{ "mtx/03", 6, Xmtx03, sizeof(Xmtx03)/sizeof(X32command)},
	{ "mtx/04", 6, Xmtx04, sizeof(Xmtx04)/sizeof(X32command)},
	{ "mtx/05", 6, Xmtx05, sizeof(Xmtx05)/sizeof(X32command)},
	{ "mtx/06", 6, Xmtx06, sizeof(Xmtx06)/sizeof(X32command)},
	{ "mtx", 3, Xmtx01, sizeof(Xmtx01)/sizeof(X32command)},
	{ "dca", 3, Xdca, sizeof(Xdca)/sizeof(X32command)},
	{ "outputs/main/01", 8, Xoutput, sizeof(Xoutput)/sizeof(X32command)},
	{ "outputs/main", 8, Xoutput, sizeof(Xoutput)/sizeof(X32command)},
	{ "outputs", 8, Xoutput, sizeof(Xoutput)/sizeof(X32command)},
	{ "headamp/000", 11, Xheadamp, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/001", 11, Xheadamp001, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/002", 11, Xheadamp002, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/003", 11, Xheadamp003, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/004", 11, Xheadamp004, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/005", 11, Xheadamp005, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/006", 11, Xheadamp006, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/007", 11, Xheadamp007, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/008", 11, Xheadamp008, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/009", 11, Xheadamp009, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/010", 11, Xheadamp010, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/011", 11, Xheadamp011, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/012", 11, Xheadamp012, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/013", 11, Xheadamp013, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/014", 11, Xheadamp014, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/015", 11, Xheadamp015, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/016", 11, Xheadamp016, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/017", 11, Xheadamp017, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/018", 11, Xheadamp018, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/019", 11, Xheadamp019, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/020", 11, Xheadamp020, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/021", 11, Xheadamp021, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/022", 11, Xheadamp022, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/023", 11, Xheadamp023, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/024", 11, Xheadamp024, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/025", 11, Xheadamp025, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/026", 11, Xheadamp026, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/027", 11, Xheadamp027, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/028", 11, Xheadamp028, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/029", 11, Xheadamp029, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/030", 11, Xheadamp030, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/031", 11, Xheadamp031, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/032", 11, Xheadamp032, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/033", 11, Xheadamp033, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/034", 11, Xheadamp034, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/035", 11, Xheadamp035, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/036", 11, Xheadamp036, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/037", 11, Xheadamp037, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/038", 11, Xheadamp038, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/039", 11, Xheadamp039, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/040", 11, Xheadamp040, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/041", 11, Xheadamp041, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/042", 11, Xheadamp042, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/043", 11, Xheadamp043, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/044", 11, Xheadamp044, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/045", 11, Xheadamp045, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/046", 11, Xheadamp046, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/047", 11, Xheadamp047, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/048", 11, Xheadamp048, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/049", 11, Xheadamp049, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/050", 11, Xheadamp050, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/051", 11, Xheadamp051, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/052", 11, Xheadamp052, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/053", 11, Xheadamp053, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/054", 11, Xheadamp054, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/055", 11, Xheadamp055, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/056", 11, Xheadamp056, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/057", 11, Xheadamp057, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/058", 11, Xheadamp058, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/059", 11, Xheadamp059, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/060", 11, Xheadamp060, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/061", 11, Xheadamp061, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/062", 11, Xheadamp062, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/063", 11, Xheadamp063, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/064", 11, Xheadamp064, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/065", 11, Xheadamp065, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/066", 11, Xheadamp066, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/067", 11, Xheadamp067, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/068", 11, Xheadamp068, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/069", 11, Xheadamp069, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/070", 11, Xheadamp070, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/071", 11, Xheadamp071, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/072", 11, Xheadamp072, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/073", 11, Xheadamp073, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/074", 11, Xheadamp074, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/075", 11, Xheadamp075, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/076", 11, Xheadamp076, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/077", 11, Xheadamp077, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/078", 11, Xheadamp078, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/079", 11, Xheadamp079, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/080", 11, Xheadamp080, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/081", 11, Xheadamp081, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/082", 11, Xheadamp082, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/083", 11, Xheadamp083, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/084", 11, Xheadamp084, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/085", 11, Xheadamp085, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/086", 11, Xheadamp086, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/087", 11, Xheadamp087, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/088", 11, Xheadamp088, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/089", 11, Xheadamp089, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/090", 11, Xheadamp090, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/091", 11, Xheadamp091, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/092", 11, Xheadamp092, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/093", 11, Xheadamp093, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/094", 11, Xheadamp094, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/095", 11, Xheadamp095, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/096", 11, Xheadamp096, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/097", 11, Xheadamp097, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/098", 11, Xheadamp098, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/099", 11, Xheadamp099, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/100", 11, Xheadamp100, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/101", 11, Xheadamp101, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/102", 11, Xheadamp102, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/103", 11, Xheadamp103, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/104", 11, Xheadamp104, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/105", 11, Xheadamp105, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/106", 11, Xheadamp106, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/107", 11, Xheadamp107, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/108", 11, Xheadamp108, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/109", 11, Xheadamp109, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/110", 11, Xheadamp110, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/111", 11, Xheadamp111, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/112", 11, Xheadamp112, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/113", 11, Xheadamp113, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/114", 11, Xheadamp114, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/115", 11, Xheadamp115, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/116", 11, Xheadamp116, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/117", 11, Xheadamp117, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/118", 11, Xheadamp118, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/119", 11, Xheadamp119, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/120", 11, Xheadamp120, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/121", 11, Xheadamp121, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/122", 11, Xheadamp122, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/123", 11, Xheadamp123, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/124", 11, Xheadamp124, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/125", 11, Xheadamp125, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/126", 11, Xheadamp126, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp/127", 11, Xheadamp127, sizeof(Xheadamp)/sizeof(X32command)},
	{ "headamp", 7, Xheadamp, sizeof(Xheadamp)/sizeof(X32command)},
	{ "-ha", 3, Xmisc, sizeof(Xmisc)/sizeof(X32command)},
	{ "-usb", 4, Xmisc, sizeof(Xmisc)/sizeof(X32command)},
	{ "undo", 4, Xdummy, sizeof(Xdummy)/sizeof(X32command)},
	{ "-action", 7, Xdummy, sizeof(Xdummy)/sizeof(X32command)},
	{ "-show/showfile/snippet", 22, Xsnippet, sizeof(Xsnippet)/sizeof(X32command)},	// !! keep
	{ "-show/showfile/scene", 20, Xscene, sizeof(Xscene)/sizeof(X32command)}, 		// in this
	{ "-show", 5, Xshow, sizeof(Xshow)/sizeof(X32command)},							// order !!
	{ "-urec", 5, Xurec, sizeof(Xurec)/sizeof(X32command)},
	{ "-libs/fx", 8, Xlibsf, sizeof(Xlibsf)/sizeof(X32command)},					// !! keep
	{ "-libs/r", 7, Xlibsr, sizeof(Xlibsr)/sizeof(X32command)},						// in this
	{ "-libs", 5, Xlibsc, sizeof(Xlibsc)/sizeof(X32command)},						// order !!
};
int Xnode_max = sizeof(Xnode) / sizeof(X32node);

X32command *node_single_command;//saved command pointer for function_node_single usage
int node_single_index;			//saved command index for function_node_single usage
			//

char r_buf[BSIZE];				// Receiving buffer
int r_len;						// receiving buffer number of bytes
void* v_buf = &(r_buf[0]);		// void ptr to avoid gcc complaining about non-strict aliasing
char s_buf[BSIZE];				// Send buffer
int s_len;						// send buffer number of bytes
int Xverbose = X32VERBOSE;		// verbose status
int X_remote = VERB_REMOTE;		// if verbose: xremote echo status
int X_batch = VERB_BATCH;		// if verbose: batchsubscribe echo status
int X_format = VERB_FORMAT;		// if verbose: formatsubscribe echo status
int X_renew = VERB_RENEW;		// if verbose: renew echo status
int X_meter = VERB_METER;		// if verbose: meter report status
int Xdebug = X32DEBUG;			// if verbose: debug status
int zero = 0;					// a value of 0 we can point to
int one = 1;					// a value to 1 we can point to
int keep_on = 1;				// main loop flag
time_t xremote_time;			// time value for /xremote command
char tmp_str[48];				// temporary string

int Xfd, p_status;
struct sockaddr_in Client_ip, Server_ip;
struct sockaddr *Client_ip_pt = (struct sockaddr*) &Client_ip;
struct sockaddr *Server_ip_pt = (struct sockaddr*) &Server_ip;
char Xport_str[8];
char Xip_str[32];

fd_set readfds;
struct timeval timeout;


struct sockaddr_in	Xip;
struct sockaddr*		Xip_addr;

struct timeval		timeout;
fd_set 				ufds;
int					p_status;

socklen_t			Xip_len = sizeof(Xip);	// length of addresses