/*
 * X32Fx.h
 *
 *  Created on: 4 févr. 2015
 *      Author: patrick
 */

#ifndef X32FX_H_
#define X32FX_H_


char*    	Ssource[] = {" OFF", " LR", " LR+C", " LRPFL", " LRAFL", " AUX56", " AUX78"};
char*    	Sosct[] =	{" SINE", " PINK", " WHITE"};
char*		Sroutin[] = {" AN1-8", " AN9-16", " AN17-24", " AN25-32",
						" A1-8", " A9-16", " A17-24", " A25-32", " A33-40", " A41-48",
						" B1-8", " B9-16", " B17-24", " B25-32", " B33-40", " B41-48",
						" CARD1-8", " CARD9-16", " CARD17-24", " CARD25-32"};

char*		Sroutax[] = {" AUX1-4", " AN1-2", " AN1-4", " AN1-6",
						" A1-2", " A1-4", " A1-6", " B1-2", " B1-4", " B1-6",
						" CARD1-2", "CARD1-4", "CARD1-6"};

char*		Sroutac[] = {" AN1-8", " AN9-16", " AN17-24", " AN25-32",
						" A1-8", " A9-16", " A17-24", " A25-32", " A33-40", " A41-48",
						" B1-8", " B9-16", " B17-24", " B25-32", " B33-40", " B41-48",
						" CARD1-8", " CARD9-16", " CARD17-24", " CARD25-32",
						" OUT1-8", " OUT9-16", " P161-8", " P169-16", " AUX1-6/Mon", " AuxIN1-6/TB"};

char*		Srouto1[] = {" AN1-4", " AN9-12", " AN17-20", " AN25-28",
						" A1-4", " A9-12", " A17-20", " A25-28", " A33-36", " A41-44",
						" B1-4", " B9-12", " B17-20", " B25-28", " B33-46", " B41-44",
						" CARD1-4", " CARD9-12", " CARD17-20", " CARD25-28",
						" OUT1-4", " OUT9-12", " P161-4", " P169-12", " AUX/CR", " AUX/TB"};

char*		Srouto2[] = {" AN5-8", " AN13-16", " AN21-24", " AN29-32",
						" A5-8", " A13-16", " A21-24", " A29-32", " A37-40", " A45-48",
						" B5-8", " B13-16", " B21-24", " B29-32", " B37-40", " B45-48",
						" CARD5-8", " CARD13-16", " CARD21-24", " CARD29-32",
						" OUT5-8", " OUT13-16", " P165-8", " P1613-16", " AUX/CR", " AUX/TB"};

char*		Scolor[] = {" OFF", " RD", " GN", " YE", " BL", " MG", " CY", " WH",
					   " OFFi", " RDi", " GNi", " YEi", " BLi", " MGi", " CYi", " WHi"};

char*		Sfslope[] = {" 12", " 18", " 24"};

char*		Samix[] = {" OFF", " X", " Y"};

char*		Sgmode[] = {" EXP2", " EXP3", " EXP4", " GATE", " DUCK"};

char*		Sgftype[] = {" LC6", " LC12", " HC6", " HC12", " 1.0", " 2.0", " 3.0", " 5.0", " 10.0"};

char*		Sdmode[] = {" COMP", " EXP"};
char*		Sddet[] = {" PEAK", " RMS"};
char*		Sdenv[] = {" LIN", " LOG"};
char*		Sdratio[] = {" 1.1", " 1.3", " 1.5", " 2.0", " 2.5", " 3.0", " 4.0", " 5.0", " 7.0",
						 " 10", " 20", " 100"};

char *		Sinsel[] = {" OFF", " FX1L", " FX1R", " FX2L", " FX2R", " FX3L", " FX3R", " FX4L", " FX4R",
		" FX5L", " FX5R", " FX6L", " FX6R", " FX7L", " FX7R", " FX8L", " FX8R", " AUX1",
		" AUX2", " AUX3", " AUX4", " AUX5", " AUX6"};

char*		Sdpos[] = {" PRE", " POST"};

char*		Setype[] = {" Lcut", " LShv", " PEQ", " VEQ", " HShv", " HCut", " BU6",
				" BU12", " BS12", " LR12", " BU18", " BU24", " BS24", " LR24"};

char*		Sctype[] = {" IN/LC", " <-EQ", " EQ->", " PRE", " POST", " GRP"};

char*		Smpos[] = {" IN/LC", " IN/LC+M", " <-EQ", " <-EQ+M", " EQ->", " EQ->+M", " PRE", " PRE+M", " POST"};

char* Sfactor[] = {" 1/4", " 3/8", " 1/2", " 2/3", " 1", " 4/3", " 3/2", " 2", " 3"};

char* Smode[] = {" ST", " X", " M"};

char* Sfmode[] = {" LP", " HP", " BP", " NO"};

char* Sfwave[] = {" TRI", " SIN", " SAW", " SAW-", " RMP", " SQU", " RND"};

char* Sfrange[] = {" LO", " MID", " HI"};

char* Sfpattern[] = {" 1/4", " 1/3", " 3/8", " 1/2", " 2/3", " 3/4", " 1", " 1/4X", " 1/3X", " 3/8X", " 1/2X", " 2/3X", " 3/4X","1X"};

char* Sfdtype[] = {" AMB", " CLUB", " HALL"};

char* Sfddelay[] = {" 1", " 1/2", " 2/3", " 3/2"};

char* Sfplfreq[] = {" 20", " 30", " 60", " 100"};

char* Sfpmfreq[] = {" 3K", " 4K", " 5K", " 8K", " 10K", " 12K", " 16K"};

char* Sfphfreq[] = {" 5K", " 10K", " 20K"};

char* Sfqlfreq[] = {" 200", " 300", " 500", " 700", " 1000"};

char* Sfqmfreq[] = {" 200", " 300", " 500", " 700", " 1k", " 1k5", " 2k", " 3k", " 4k", " 5k", " 7k"};

char* Sfqhfreq[] = {" 1k5", " 2k", " 3k", " 4k", " 5k"};

char* Sflcmb[] = {" OFF", " Bd1", " Bd2", " Bd3", " Bd4", " Bd5"};

char* Sfrcmb[] = {" 1.1", " 1.2", " 1.3", " 1.5", " 1.7", " 2", " 2.5", " 3", " 3.5", " 4", " 5", " 7", " 10", " LIM"};

char* Sfmcmb[] = {" GR", " SBC", " PEAK"};

char* Sfrulc[] = {" 4", " 8", " 12", " 20", " ALL"};

char* f201[] = {" 20.0", " 20.7", " 21.4", " 22.2", " 23.0", " 23.8", " 24.6", " 25.5", " 26.4", " 27.3",
		" 28.3", " 29.2", " 30.3", " 31.3", " 32.4", " 33.6", " 34.8", " 36.0", " 37.2", " 38.6",
		" 39.9", " 41.3", " 42.8", " 44.3", " 45.8", " 47.4", " 49.1", " 50.8", " 52.6", " 54.5",
		" 56.4", " 58.3", " 60.4", " 62.5", " 64.7", " 67.0", " 69.3", " 71.8", " 74.3", " 76.9",
		" 79.6", " 82.4", " 85.3", " 88.3", " 91.4", " 94.6", " 98.0", " 101.4", " 105.0", " 108.7",
		" 112.5", " 116.4", " 120.5", " 124.7", " 129.1", " 133.7", " 138.4", " 143.2", " 148.3", " 153.5",
		" 158.9", " 164.4", " 170.2", " 176.2", " 182.4", " 188.8", " 195.4", " 202.3", " 209.4", " 216.8",
		" 224.4", " 232.3", " 240.5", " 248.9", " 257.6", " 266.7", " 276.1", " 285.8", " 295.8", " 306.2",
		" 317.0", " 328.1", " 339.6", " 351.6", " 363.9", " 376.7", " 390.0", " 403.7", " 417.9", " 432.5",
		" 447.7", " 463.5", " 479.8", " 496.6", " 514.1", " 532.1", " 550.8", " 570.2", " 590.2", " 611.0",
		" 632.5", " 654.7", " 677.7", " 701.5", " 726.2", " 751.7", " 778.1", " 805.4", " 833.7", " 863.0",
		" 893.4", " 924.8", " 957.3", " 990.9", " 1k02", " 1k06", " 1k09", " 1k13", " 1k17", " 1k21",
		" 1k26", " 1k30", " 1k35", " 1k39", " 1k44", " 1k49", " 1k55", " 1k60", " 1k66", " 1k72",
		" 1k78", " 1k84", " 1k90", " 1k97", " 2k04", " 2k11", " 2k19", " 2k27", " 2k34", " 2k43",
		" 2k51", " 2k60", " 2k69", " 2k79", " 2k89", " 2k99", " 3k09", " 3k20", " 3k31", " 3k43",
		" 3k55", " 3k68", " 3k81", " 3k94", " 4k08", " 4k22", " 4k37", " 4k52", " 4k68", " 4k85",
		" 5k02", " 5k20", " 5k38", " 5k57", " 5k76", " 5k97", " 6k18", " 6k39", " 6k62", " 6k85",
		" 7k09", " 7k34", " 7k60", " 7k87", " 8k14", " 8k43", " 8k73", " 9k03", " 9k35", " 9k68",
		" 10k02", " 10k37", " 10k74", " 11k11", " 11k50", " 11k91", " 12k33", " 12k76", " 13k21", " 13k67",
		" 14k15", " 14k65", " 15k17", " 15k70", " 16k25", " 16k82", " 17k41", " 18k03", " 18k66", " 19k32",
		" 20k00"};

char* f121[] = {" 20.0", " 21.2", " 22.4", " 23.8", " 25.2", " 26.7", " 28.3", " 29.9", " 31.7", " 33.6",
		" 35.6", " 37.7", " 39.9", " 42.3", " 44.8", " 47.4", " 50.2", " 53.2", " 56.4", " 59.7",
		" 63.2", " 67.0", " 71.0", " 75.2", " 79.6", " 84.3", " 89.3", " 94.6", " 100.2", " 106.2",
		" 112.5", " 119.1", " 126.2", " 133.7", " 141.6", " 150.0", " 158.9", " 168.3", " 178.3", " 188.8",
		" 200.0", " 211.9", " 224.4", " 237.7", " 251.8", " 266.7", " 282.5", " 299.2", " 317.0", " 335.8",
		" 355.7", " 376.7", " 399.1", " 422.7", " 447.7", " 474.3", " 502.4", " 532.1", " 563.7", " 597.1",
		" 632.5", " 669.9", " 709.6", " 751.7", " 796.2", " 843.4", " 893.4", " 946.3", " 1k00", " 1k06",
		" 1k12", " 1k19", " 1k26", " 1k33", " 1k41", " 1k49", " 1k58", " 1k68", " 1k78", " 1k88",
		" 2k00", " 2k11", " 2k24", " 2k37", " 2k51", " 2k66", " 2k82", " 2k99", " 3k16", " 3k35",
		" 3k55", " 3k76", " 3k99", " 4k22", " 4k47", " 4k74", " 5k02", " 5k32", " 5k63", " 5k97",
		" 6k32", " 6k69", " 7k09", " 7k51", " 7k96", " 8k43", " 8k93", " 9k46", " 10k02", " 10k61",
		" 11k24", " 11k91", " 12k61", " 13k36", " 14k15", " 14k99", " 15k88", " 16k82", " 17k82", " 18k88",
		" 20k00"};

char* f101[] = {" 20.0", " 20.6", " 21.2", " 21.9", " 22.5", " 23.2", " 23.9", " 24.7", " 25.4", " 26.2",
		" 27.0", " 27.8", " 28.7", " 29.5", " 30.4", " 31.3", " 32.3", " 33.3", " 34.3", " 35.3",
		" 36.4", " 37.5", " 38.7", " 39.8", " 41.0", " 42.3", " 43.6", " 44.9", " 46.3", " 47.7",
		" 49.1", " 50.6", " 52.2", " 53.7", " 55.4", " 57.1", " 58.8", " 60.6", " 62.4", " 64.3",
		" 66.3", " 68.3", " 70.4", " 72.5", " 74.7", " 77.0", " 79.3", " 81.8", " 84.2", " 86.8",
		" 89.4", " 92.2", " 95.0", " 97.9", " 100.8", " 103.9", " 107.1", " 110.3", " 113.7", " 117.1",
		" 120.7", " 124.4", " 128.1", " 132.0", " 136.0", " 140.2", " 144.4", " 148.8", " 153.4", " 158.0",
		" 162.8", " 167.8", " 172.9", " 178.1", " 183.6", " 189.1", " 194.9", " 200.8", " 206.9", " 213.2",
		" 219.7", " 226.4", " 233.3", " 240.4", " 247.7", " 255.2", " 263.0", " 271.0", " 279.2", " 287.7",
		" 296.5", " 305.5", " 314.8", " 324.3", " 334.2", " 344.4", " 354.8", " 365.6", " 376.7", " 388.2",
		" 400.0"};

char* Sflookup[] = {
		 "ffffffffffff----------------------------------------------------", /* HALL */
		 "ffffffffff------------------------------------------------------", /* AMBI */
		 "ffffffffffffffff------------------------------------------------", /* RPLT */
		 "ffffffffffffffff------------------------------------------------", /* ROOM */
		 "ffffffffffffffff------------------------------------------------", /* CHAM */
		 "ffffffffffff----------------------------------------------------", /* PLAT */
		 "fffiifffff------------------------------------------------------", /* VREV */
		 "ffffffffffffi---------------------------------------------------", /* VRM */
		 "ffffffffff------------------------------------------------------", /* GATE */
		 "fffffffff-------------------------------------------------------", /* RVRS */
		 "ffiiifffffff----------------------------------------------------", /* DLY */
		 "ffffffiffiffiii-------------------------------------------------", /* _3TAP */
		 "ffffffifififiii-------------------------------------------------", /* _4TAP */
		 "fffffffffff-----------------------------------------------------", /* CRS */
		 "ffffffffffff----------------------------------------------------", /* FLNG */
		 "ffffffffffff----------------------------------------------------", /* PHAS */
		 "iiiiiii---------------------------------------------------------", /* DIMC */
		 "ffffififffffii--------------------------------------------------", /* FILT */
		 "ffffffii--------------------------------------------------------", /* ROTA */
		 "fffffffff-------------------------------------------------------", /* PAN */
		 "iifffiifff------------------------------------------------------", /* SUB */
		 "fiffffffffff----------------------------------------------------", /* D_RV */
		 "ffffffffffff----------------------------------------------------", /* CR_R */
		 "ffffffffffff----------------------------------------------------", /* FL_R */
		 "fiffffffffff----------------------------------------------------", /* D_CR */
		 "fiffffffffff----------------------------------------------------", /* D_FL */
		 "fifffffiiffff---------------------------------------------------", /* MODD */
		 "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", /* GEQ2 */
		 "ffffffffffffffffffffffffffffffff--------------------------------", /* GEQ */
		 "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", /* TEQ2 */
		 "ffffffffffffffffffffffffffffffff--------------------------------", /* TEQ */
		 "ffffii----------------------------------------------------------", /* DES2 */
		 "ffffii----------------------------------------------------------", /* DES */
		 "iffifffifii-----------------------------------------------------", /* P1A */
		 "iffifffifiiiffifffifii------------------------------------------", /* P1A2 */
		 "ififififi-------------------------------------------------------", /* PQ5 */
		 "ififififiififififi----------------------------------------------", /* PQ5S */
		 "ffffff----------------------------------------------------------", /* WAVD */
		 "ffffffii--------------------------------------------------------", /* LIM */
		 "iifffififiiffffiffiffiffiffii-----------------------------------", /* CMB */
		 "iifffififiiffffiffiffiffiffiiiifffififiiffffiffiffiffiffii------", /* CMB2 */
		 "iffffff---------------------------------------------------------", /* FAC */
		 "iffffffiffffff--------------------------------------------------", /* FAC1M */
		 "iffffffiffffff--------------------------------------------------", /* FAC2 */
		 "iffif-----------------------------------------------------------", /* LEC */
		 "iffififfif------------------------------------------------------", /* LEC2 */
		 "iffffi----------------------------------------------------------", /* ULC */
		 "iffffiiffffi----------------------------------------------------", /* ULC2 */
		 "ffffffffiffffffffi----------------------------------------------", /* ENH2 */
		 "ffffffffi-------------------------------------------------------", /* ENH */
		 "ffffffiffffffi--------------------------------------------------", /* EXC2 */
		 "ffffffi---------------------------------------------------------", /* EXC */
		 "fffffff---------------------------------------------------------", /* IMG */
		 "iiifffff--------------------------------------------------------", /* EDI */
		 "ifffifff--------------------------------------------------------", /* SON */
		 "ffffffffiffffffffi----------------------------------------------", /* AMP2 */
		 "ffffffffi-------------------------------------------------------", /* AMP */
		 "ffffffffffffffffffff--------------------------------------------", /* DRV2 */
		 "ffffffffff------------------------------------------------------", /* DRV */
		 "ffffffffffff----------------------------------------------------", /* PIT2 */
		 "ffffff----------------------------------------------------------", /* PIT */
};

char* Sflookup2[] = {
		 "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", /* GEQ2 */
		 "ffffffffffffffffffffffffffffffff--------------------------------", /* GEQ */
		 "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", /* TEQ2 */
		 "ffffffffffffffffffffffffffffffff--------------------------------", /* TEQ */
		 "ffffii----------------------------------------------------------", /* DES2 */
		 "ffffii----------------------------------------------------------", /* DES */
		 "iffifffifii-----------------------------------------------------", /* P1A */
		 "iffifffifiiiffifffifii------------------------------------------", /* P1A2 */
		 "ififififi-------------------------------------------------------", /* PQ5 */
		 "ififififiififififi----------------------------------------------", /* PQ5S */
		 "ffffff----------------------------------------------------------", /* WAVD */
		 "ffffffii--------------------------------------------------------", /* LIM */
		 "iifffififiiffffiffiffiffiffii-----------------------------------", /* CMB */
		 "iifffififiiffffiffiffiffiffiiiifffififiiffffiffiffiffiffii------", /* CMB2 */
		 "iffffff---------------------------------------------------------", /* FAC */
		 "iffffffiffffff--------------------------------------------------", /* FAC1M */
		 "iffffffiffffff--------------------------------------------------", /* FAC2 */
		 "iffif-----------------------------------------------------------", /* LEC */
		 "iffififfif------------------------------------------------------", /* LEC2 */
		 "iffffi----------------------------------------------------------", /* ULC */
		 "iffffiiffffi----------------------------------------------------", /* ULC2 */
		 "ffffffffiffffffffi----------------------------------------------", /* ENH2 */
		 "ffffffffi-------------------------------------------------------", /* ENH */
		 "ffffffiffffffi--------------------------------------------------", /* EXC2 */
		 "ffffffi---------------------------------------------------------", /* EXC */
		 "fffffff---------------------------------------------------------", /* IMG */
		 "iiifffff--------------------------------------------------------", /* EDI */
		 "ifffifff--------------------------------------------------------", /* SON */
		 "ffffffffiffffffffi----------------------------------------------", /* AMP2 */
		 "ffffffffi-------------------------------------------------------", /* AMP */
		 "ffffffffffffffffffff--------------------------------------------", /* DRV2 */
		 "ffffffffff------------------------------------------------------", /* DRV */
		 "ffffffffffff----------------------------------------------------", /* PHAS */
		 "ffffififffffii--------------------------------------------------", /* FILT */
		 "fffffffff-------------------------------------------------------", /* PAN */
		 "iifffiifff------------------------------------------------------", /* SUB */
};


X32command Xfx1[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/1",					{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/1/type",			{E32}, F_XET, {0}, Sfxtyp1},
		{"/fx/1/source",			{FXSRC}, F_FND, {0}, NULL},
		{"/fx/1/source/l",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/1/source/r",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/1/par",				{FXPAR1}, F_FND, {0}, NULL},
		{"/fx/1/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/1/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx1_max = sizeof(Xfx1) / sizeof(X32command);


X32command Xfx2[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/2",					{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/2/type",			{E32}, F_XET, {0}, Sfxtyp1},
		{"/fx/2/source",			{FXSRC}, F_FND, {0}, NULL},
		{"/fx/2/source/l",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/2/source/r",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/2/par",				{FXPAR1}, F_FND, {0}, NULL},
		{"/fx/2/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/2/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx2_max = sizeof(Xfx2) / sizeof(X32command);


X32command Xfx3[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/3",					{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/3/type",			{E32}, F_XET, {0}, Sfxtyp1},
		{"/fx/3/source",			{FXSRC}, F_FND, {0}, NULL},
		{"/fx/3/source/l",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/3/source/r",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/3/par",				{FXPAR1}, F_FND, {0}, NULL},
		{"/fx/3/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/3/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx3_max = sizeof(Xfx3) / sizeof(X32command);


X32command Xfx4[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/4",					{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/4/type",			{E32}, F_XET, {0}, Sfxtyp1},
		{"/fx/4/source",			{FXSRC}, F_FND, {0}, NULL},
		{"/fx/4/source/l",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/4/source/r",		{E32}, F_XET, {0}, Sfxsrc},
		{"/fx/4/par",				{FXPAR1}, F_FND, {0}, NULL},
		{"/fx/4/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/4/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx4_max = sizeof(Xfx4) / sizeof(X32command);


X32command Xfx5[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/5",					{FXTYP2}, F_FND, {0}, NULL},
		{"/fx/5/type",			{E32}, F_XET, {0}, Sfxtyp2},
		{"/fx/5/par",				{FXPAR2}, F_FND, {0}, NULL},
		{"/fx/5/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/5/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx5_max = sizeof(Xfx5) / sizeof(X32command);


X32command Xfx6[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/6",					{FXTYP2}, F_FND, {0}, NULL},
		{"/fx/6/type",			{E32}, F_XET, {0}, Sfxtyp2},
		{"/fx/6/par",				{FXPAR2}, F_FND, {0}, NULL},
		{"/fx/6/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/6/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx6_max = sizeof(Xfx6) / sizeof(X32command);


X32command Xfx7[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/7",					{FXTYP2}, F_FND, {0}, NULL},
		{"/fx/7/type",			{E32}, F_XET, {0}, Sfxtyp2},
		{"/fx/7/par",				{FXPAR2}, F_FND, {0}, NULL},
		{"/fx/7/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/7/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx7_max = sizeof(Xfx7) / sizeof(X32command);


X32command Xfx8[] = {
		{"/fx",						{FXTYP1}, F_FND, {0}, NULL},
		{"/fx/8",					{FXTYP2}, F_FND, {0}, NULL},
		{"/fx/8/type",			{E32}, F_XET, {0}, Sfxtyp2},
		{"/fx/8/par",				{FXPAR2}, F_FND, {0}, NULL},
		{"/fx/8/par/01",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/02",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/03",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/04",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/05",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/06",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/07",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/08",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/09",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/10",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/11",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/12",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/13",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/14",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/15",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/16",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/17",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/18",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/19",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/20",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/21",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/22",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/23",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/24",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/25",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/26",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/27",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/28",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/29",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/30",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/31",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/32",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/33",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/34",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/35",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/36",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/37",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/38",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/39",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/40",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/41",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/42",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/43",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/44",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/45",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/46",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/47",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/48",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/49",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/50",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/51",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/52",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/53",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/54",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/55",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/56",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/57",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/58",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/59",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/60",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/61",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/62",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/63",		{FX32}, F_XET, {0}, NULL},
		{"/fx/8/par/64",		{FX32}, F_XET, {0}, NULL},
};
int Xfx8_max = sizeof(Xfx8) / sizeof(X32command);


X32command	*Xfxset[8] = {
		Xfx1, Xfx2, Xfx3, Xfx4, Xfx5, Xfx6, Xfx7, Xfx8,
}; 


#endif /* X32FX_H_ */
