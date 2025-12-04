#pragma once


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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include <map>

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



#include "X32const.h"

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



		


// ###########################################################
// 
// ##      ## ########    #####   
//  ##    ##         ## ##     ## 
//   ##  ##          ##        ## 
//    ####     #######       ##   
//   ##  ##          ##   ##      
//  ##    ##         ## ##        
// ##      ## ########  ######### 
// 
// ###########################################################

class X32 {

	private:
		int Xheader_max = 37;

		socklen_t Client_ip_len = sizeof(Client_ip);	// length of addresses

		void Xsend(int who_to);
		void X32Print(struct X32command* command);
		int X32Shutdown();
		int X32Init();
		int FXc_lookup(X32command* command, int i);
		int funct_params(X32command* command, int i);
		

		char* Slevel(float fin);
		char* Slinf(float fin, float fmin, float fmax, int pre);
		char* Slinfs(float fin, float fmin, float fmax, int pre);
		char* Slogf(float fin, float fmin, float fmax, int pre);	
		char* Sbitmp(int iin, int len);
		char* Sint(int iin);


		int Xmeters_max = sizeof(Xmeters) / sizeof(X32command);
		int Xlibsf_max = sizeof(Xlibsf) / sizeof(X32command);

	public:

		int x32_startup();
		void getmyIP();

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
		char* REnum(X32command* command, char* str_pt_in, const char* str_enum[]);

		int function_node_single(int i_value);
		void GetFxPar1(X32command* command, char* buf, int ipar, int type);
		void SetFxPar1(X32command* command, char* str_pt_in, int ipar, int type);
		void Xprepmeter(int i, int l, char *buf, int n, int k);



		void Xdump(char *buf, int len, int debug);
		void Xfdump(char *header, char *buf, int len, int debug);
		void Xsdump(char *str_out, char *buf, int len);

		int Xsprint(char *bd, int index, char format, const void *bs);
		int Xfprint(char *bd, int index, char* text, char format, void *bs);



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
		int function_renew();
		int function_libs();
		int function_showdump();
		int function();

		


		


	
	
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
		struct sockaddr_in Xip;
		struct sockaddr* Xip_addr;

		fd_set ufds;
		socklen_t Xip_len = sizeof(Xip);	// length of addresses



};



