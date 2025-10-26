//
// X32.c
//
// Created on: 16 janv. 2015
// Author: Patrick-Gilles Maillot
//
// Revisions: 0.30: changed "libch" to "libchan" for /save /load/ /delete etc.,
//		/save for 'scene' is a ,siss type function, not ,sissi
//		and "channel" to "libchan" in /copy
//		added a status being sent after /copy
// 0.40 & 0.41: Accepts OSC empty Tag Strings to better conform to OSC 1.1 standard
// 0.50: updated parameters accepted for /-stat, /-prefs, /-action commands
// 0.60: better handling of /node commands (/node ,s ch), (/node ,s fxrtn), etc...
// 0.65: support for (some) 3.04 features
// 0.66: fixed typos and cosmetic changes
// 0.67: fixed bug in the reply sent by /status ; /bus/02/mix/01/type fixed
// 0.68: fixed bug in getting IP address with non-Windows systems (Xport_str was over written)
// 0.69: support for X-Live! and 3.08FW
// 0.70: support for /node single argument commands such as /node ,s -prefs/rta/visibility, or /node ,s headamp/006/phantom
// 0.71: support for / commands such as / ,s "/ch/01/config/name toto", support for meters, and generally now answering
//       correcly to other remote clients on complex changes, i.e. if a data is not changed, it will not be sent to remote
//       clients.
// 0.72: bug fixes
// 0.73: better handling of float comaparisons (using EPSILON)
// 0.74: new functions /-libs, bug fixes
// 0.75: new f-i flag to select IP address and changes in the way the bind() is done
// 0.76: /-prefs/name correctly updates the name of the console (in /info and /status)
// 0.77: fixed incorrect index computation in the case of mutliple tags, ex: /config/mute ,iiiiii would not set last value
// 0.78: includes FW4.0 capabilities, and includes /-stat/lock ,i 2 as a shutdown command
// 0.79: /config/osc/dest was missing as a command
// 0.80: error in /fx/... command
// 0.81: added some elements related to 4.06 (/mtx/../grp, /main/st|m/grp,..) tested with xedit 4.12
// 0.82: added some elements related to 4.06 (/-stat/dcaspill)
// 0.83: fixed bugs in FX parameters lists and ranges
// 0.84: additional controls on strip parameter ranges to avoid seg faults.
// 0.85: Fixed parameter type s in /mtx/../dyn/thr and .../dyn/filter/f
// 0.86: Fixed missing comma in XiQeq[] definitions
// 0.87: Partial fix for /meters command
// 0.88: Fix for /meters/5 and /meter/6 timefactor control
//

#include "X32.h"



#ifdef __WIN32__
WSADATA wsa;
int Client_ip_len = sizeof(Client_ip);			// length of addresses
INT WSAAPI getaddrinfo(char *pNodeName, char *pServiceName, struct addrinfo *pHints, struct addrinfo **ppResult);
#else
socklen_t Client_ip_len = sizeof(Client_ip);	// length of addresses
#endif

// int main(int argc, char **argv) {
// 	int i, whoto, noIP;
// 	int input_intch;
// 	struct addrinfo hints;
// 	struct addrinfo *result, *rp;
// //
// // Manage arguments
// 	fflush(stdout);
// 	noIP = 1;
// 	while ((input_intch = getopt(argc, argv, "i:d:v:x:b:f:r:m:h")) != -1) {
// 		switch ((char)input_intch) {
// 		case 'i':
// 			strcpy(Xip_str, optarg );
// 			noIP = 0;
// 			break;
// 		case 'd':
// 			sscanf(optarg, "%d", &Xdebug);
// 			break;
// 		case 'v':
// 			sscanf(optarg, "%d", &Xverbose);
// 			break;
// 		case 'x':
// 			sscanf(optarg, "%d", &X_remote);
// 			break;
// 		case 'b':
// 			sscanf(optarg, "%d", &X_batch);
// 			break;
// 		case 'f':
// 			sscanf(optarg, "%d", &X_format);
// 			break;
// 		case 'r':
// 			sscanf(optarg, "%d", &X_renew);
// 			break;
// 		case 'm':
// 			sscanf(optarg, "%d", &X_meter);
// 			break;
// 		default:
// 		case 'h':
// 			printf("usage: X32 [-i <IP address>] - default: first IP available on system\n");
// 			printf(" [-d 0/1, debug option] - default: 0\n");
// 			printf(" [-v 0/1, verbose option] - default: 1\n");
// 			printf(" The options below apply in conjunction with -v 1\n");
// 			printf("     [-x 0/1, echoes incoming verbose for /xremote] - default: 0\n");
// 			printf("     [-b 0/1, echoes incoming verbose for /batchsubscribe] - default: 0\n");
// 			printf("     [-f 0/1, echoes incoming verbose for /formatsubscribe] - default: 0\n");
// 			printf("     [-r 0/1, echoes incoming verbose for /renew] - default: 0\n");
// 			printf("     [-m 0/1, echoes incoming verbose for /meters] - default: 0\n\n");
// 			printf(" The (non-Behringer) command \"/shutdown\" will save data and quit\n");
// 			return (0);
// 			break;
// 		}
// 	}
// // Initiate timers
// 	xremote_time = time(NULL);
// 	for (i = 0; i < MAX_METERS; i++) {
// 		gettimeofday(&XTimerMeters[i], NULL);
// 		XInterMeters[i] = XTimerMeters[i];
// 		XDeltaMeters[i] = 50000;
// 		XActiveMeters = 0;
// 	}
// // port[] = "10023" // 10023: X32 desk, 10024: XAir18
// 	strcpy(Xport_str, "10023");
// //
// 	for (i = 0; i < MAX_CLIENTS; i++) {
// 		X32Client[i].vlid = 0;
// 		X32Client[i].xrem = 0;
// 	}
// //
// #ifdef __WIN32__
// //Initialize winsock
// 	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
// 		perror("WSA Startup Error");
// 		exit(EXIT_FAILURE);
// 	}
// #endif
// //
// 	r_len = 0;
// 	printf("X32 - v0.88 - An X32 Emulator - (c)2014-2019 Patrick-Gilles Maillot\n");
// 	//
// 	// Get or use IP address
// 	if (noIP) {
// 		// Try to get an IP on this system (the first one is OK...otherwise, use -i option!)
// 		getmyIP();
// 	}
// 	// We now have an IP address in Xip_str; test it, create a sock and bind to it if OK
// 	memset(&hints, 0, sizeof(struct addrinfo));
// 	hints.ai_socktype = SOCK_DGRAM;
// 	hints.ai_family = AF_INET;
// 	hints.ai_protocol = IPPROTO_UDP;
// 	if ((i = getaddrinfo(Xip_str, Xport_str, &hints, &result)) != 0) {
// 		printf("Error getaddrinfo: %s\n", gai_strerror(i));
// 		exit(0);
// 	}
// 	noIP = 1;	/* set error state flag */
// 	for (rp = result; rp != NULL; rp = rp->ai_next) {
// 		if ((Xfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) >= 0) {
// 			if (bind(Xfd, rp->ai_addr, rp->ai_addrlen) == 0) {
// 				noIP = 0;			/* Success !*/
// 				break;
// 			}
// 			close(Xfd);
// 		}
// 	}
// 	if (noIP) {
// 		printf("Error on IP address: %s - cannot run\n", Xip_str);
// 		exit(1);
// 	} else {
// 		printf("Listening to port: %s, X32 IP = %s\n", Xport_str, Xip_str);
// 	}
// 	// read initial data (if exists)
// 	if (X32Init()) {
// 		printf("X32 resource file does not exist, create one with '/shutdown' command\n");
// 		// set IP address in local structure
// 		// search for /-prefs/ip/addr dataset and set IP address in 4 consecutine ints
// 		for (i = 0; i < Xprefs_max; i++) {
// 			if (strcmp("/-prefs/ip/addr/0", Xprefs[i].command) == 0) {
// 				sscanf(Xip_str, "%d.%d.%d.%d",	&Xprefs[i].value.ii,
// 												&Xprefs[i+1].value.ii,
// 												&Xprefs[i+2].value.ii,
// 												&Xprefs[i+3].value.ii);
// 				break;
// 			}
// 		}
// 	}
// 	// Wait for messages from client, with non blocking socket
// 	timeout.tv_sec = 0;
// 	timeout.tv_usec = 10000; //Set timeout for non blocking recvfrom(): 10ms
// 	while (keep_on) { // Main, receiving loop (active as long as keep_on is 1)
// 		whoto = 0;
// 		FD_ZERO(&readfds);
// 		FD_SET(Xfd, &readfds);
// 		p_status = select(Xfd + 1, &readfds, NULL, NULL, &timeout);
// 		if ((p_status = FD_ISSET(Xfd, &readfds)) < 0) {
// 			printf("Error while receiving\n");
// 			exit(1);
// 		}
// 		if (p_status > 0) {
// 			// We have received data - process it!
// 			// and if appropriate, reply to the address received into Client addr (ip_pt)
// 			r_len = recvfrom(Xfd, r_buf, BSIZE, 0, Client_ip_pt, &Client_ip_len);
// 			r_buf[r_len] = 0;
// 			if (Xverbose) {
// 				if (strncmp(r_buf, "/xre", 4) == 0) {
// 					if (X_remote) {
// 						Xfdump("->X", r_buf, r_len, Xdebug);
// 						fflush(stdout);
// 					}
// 				} else if (strncmp(r_buf, "/bat", 4) == 0) {
// 					if (X_batch) {
// 						Xfdump("->X", r_buf, r_len, Xdebug);
// 						fflush(stdout);
// 					}
// 				} else if (strncmp(r_buf, "/for", 4) == 0) {
// 					if (X_format) {
// 						Xfdump("->X", r_buf, r_len, Xdebug);
// 						fflush(stdout);
// 					}
// 				} else if (strncmp(r_buf, "/ren", 4) == 0) {
// 					if (X_renew) {
// 						Xfdump("->X", r_buf, r_len, Xdebug);
// 						fflush(stdout);
// 					}
// 				} else if (strncmp(r_buf, "/met", 4) == 0) {
// 					if (X_meter) {
// 						Xfdump("->X", r_buf, r_len, Xdebug);
// 						fflush(stdout);
// 					}
// 				} else {
// 					Xfdump("->X", r_buf, r_len, Xdebug);
// 					fflush(stdout);
// 				}
// 			}
// 			// We have data coming in - Parse!
// 			i = s_len = p_status = 0;
// 			// Parse the command; this will update the Send buffer (and send buffer number of bytes)
// 			// and the parsing status in p_status
// 			while (i < Xheader_max) {
// 				if (Xheader[i].header.icom == (int) *((int*) v_buf)) { // single int test!
// 					whoto = Xheader[i].fptr(); // call associated parsing function
// 					break; // Done parsing, exit parsing while loop
// 				}
// 				i += 1;
// 			}
// 			// Done receiving/managing command parameters;
// 			Xsend(whoto);
// 		}
// #ifdef __linux__
// 		else
// 		{
// 			usleep( 10 );
// 		}
// #endif
// 		//
// 		// Update current client with data to be sent, or meters & subscribes?
// //		if (whoto == 0) {  // Meters or other data to send?
// 			gettimeofday (&xmeter_time, NULL);
// 			if (XActiveMeters) {
// 				for (i = 0; i < MAX_METERS; i++) {
// 					if (XActiveMeters & (1 << i)) {
// 						if(timercmp(&XTimerMeters[i], &xmeter_time, > )) {
// 							if(timercmp(&xmeter_time, &XInterMeters[i], > )) {
// 								if (sendto(Xfd, &Xbuf_meters[i][0], Lbuf_meters[i], 0, &XClientMeters[i], Client_ip_len) < 0) {
// 									perror("Error while sending data");
// 									return(0);
// 								}
// 								timerincrement(&XInterMeters[i], XDeltaMeters[i]);
// 							}
// 						} else {
// 							XActiveMeters &= ~(1 << i);			// set meters inactive
// 						}
// 					}
// 				}
// 			}
// //		}
// 	}
// 	return 0;
// }

// #ifdef __WIN32__
// void getmyIP() {
// 	char **pp = NULL;
// 	struct hostent *host = NULL;

// 	if (!gethostname(r_buf, 256) && (host = gethostbyname(r_buf)) != NULL) {
// 		for (pp = host->h_addr_list; *pp != NULL; pp++) {
// 			strcpy(Xip_str, (inet_ntoa(*(struct in_addr *) *pp))); // copy IP (string) address to r_buf
// 			return;
// 		}
// 	}
// 	return;
// }
// #else
// void getmyIP() {

// 	struct ifaddrs *ifaddr, *ifa;
// 	int s;

// 	r_buf[0] = 0;

// 	if (getifaddrs(&ifaddr) == -1) {
// 		perror("getifaddrs");
// 		return;
// 	}
// 	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
// 		if (ifa->ifa_addr != NULL) {
// 			// use r_buf as we may need a large string
// 			if ((s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), r_buf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST)) == 0) {
// // you typically have to replace "en0" by "wlan0", "eth0",... depending on your physical interface support
// 				if ((strcmp(ifa->ifa_name,"en0") == 0) && (ifa->ifa_addr->sa_family == AF_INET)) {
//  //printf("\tInterface : <%s>\n",ifa->ifa_name );
//  //printf("\t Address : <%s>\n", r_buf);
// 					strcpy(Xip_str, r_buf); // update Xip_str
// 					freeifaddrs(ifaddr);
// 					return;
// 				}
// 			}
// 		}
// 	}
// 	freeifaddrs(ifaddr);
// 	return;
// }
// #endif

//
//X32Print: a utility print for commands
void X32Print(struct X32command* command) {
	printf("X32-Command: %s data: ", command->command);
//
	if ((command->format.typ == I32) || (command->format.typ == E32) || (command->format.typ == P32)) {
		printf("[%6d]\n", command->value.ii);
	} else if (command->format.typ == F32) {
		if (command->value.ff < 10.) printf("[%6.4f]\n", command->value.ff);
		else if (command->value.ff < 100.) printf("[%6.3f]\n", command->value.ff);
		else if (command->value.ff < 1000.) printf("[%6.2f]\n", command->value.ff);
		else if (command->value.ff < 10000.) printf("[%6.1f]\n", command->value.ff);
		else printf("[%6f]\n", command->value.ff);
	} else if (command->format.typ == S32) {
		printf("%s\n", command->value.str);
	} else if (command->format.typ == B32) {
		printf("blob at address %08x\n", (unsigned int) command->value.ii); // casts .dta to int
	} else {
		printf("composed type at address %08x\n", (unsigned int) command->format.typ); // casts .str to int
	}
}

//
// Xsend: X32 sending data (as the result of a command or a change for example)
void Xsend(int who_to) {
	int i;


	if ((who_to & S_SND) && s_len) {
		if (Xverbose) {
			Xfdump("X->", s_buf, s_len, Xdebug);
			fflush(stdout);
		}
		if (sendto(Xfd, s_buf, s_len, 0, Client_ip_pt, Client_ip_len) < 0) {
			perror("Error while sending data");
			return;
		}
	}
	// Other clients to update based on their xremote status?
	if ((who_to & S_REM) && s_len) {
		xremote_time = time(NULL);
		for (i = 0; i < MAX_CLIENTS; i++) {
			if ((X32Client[i].vlid) && (strcmp(X32Client[i].sock.sa_data, Client_ip_pt->sa_data) != 0)) {
				if (X32Client[i].xrem > xremote_time) {
					if (Xverbose) {
						Xfdump("X->", s_buf, s_len, Xdebug);
						fflush(stdout);
					}
					if (sendto(Xfd, s_buf, s_len, 0, &(X32Client[i].sock), Client_ip_len) < 0) {
						perror("Error while sending data");
						return;
					}
//					if (strncmp(s_buf, "/-stat/solosw/", 14) == 0) {
//						// a solosw command was issued, follow with a /-stat/solo command
//						// TODO: 1 if any solosw is still on, 0 otherwise
//						sendto(Xfd, "/-stat/solo\0,i\0\0\0\0\0\1", 20, 0, &(X32Client[i].sock), Client_ip_len);
//					}
				}
			}
		}
	}
}

//
// FXc_lookup: find the parameter type of an FX parameter
int FXc_lookup(X32command* Xfx, int index) {
	int ipar, ityp;
	char ctyp;
// lookup function to find the parameter type of an FX parameter for command at index
// the function returns I32, F32, S32, B32, or E32, and NIL if an error is detected
//
// Command at index is like: /fx/<n>/par/<pp>
// we get the FX type at index -<pp> - 5
	ipar = (Xfx[index].command[10] - 48) * 10 + Xfx[index].command[11] - 48 - 1;
	if ((ipar < 0) || (ipar > 63)) return NIL;
// 2 cases to consider depending on <n>
	if (Xfx[index].command[4] < 53) {
		ityp = Xfx[index - ipar - 5].value.ii;
		ctyp = *(Sflookup[ityp] + ipar); // /fx/1/... to /fx/4/
	} else {
		ityp = Xfx[index - ipar - 2].value.ii;
		ctyp = *(Sflookup2[ityp] + ipar); // /fx/5/... to /fx/8/
	}
	switch (ctyp) {
	case 'i':
		return I32;
		break;
	case 'f':
		return F32;
		break;
	case 'e':
		return E32;
		break;
	case 's':
		return S32;
		break;
	default:
		return F32;
		break;
	}
	return NIL;
}

//
// Slevel: returns db level [-oo...10] from float[0..1]
char* Slevel(float fin) {
	float fl;

	if (fin <= 0.) {
		sprintf(snode_str, " -oo");
	} else {
		if (fin <= 0.0625) fl = 30. / 0.0625 * fin - 90.;
		else if (fin <= 0.25) fl = 30. / (0.25 - 0.0625) * (fin - 0.0625) - 60.;
		else if (fin < 0.5) fl = 20. / (0.5 - 0.25) * (fin - 0.25) - 30.;
		else fl = 20. / (1. - 0.5) * (fin - 0.5) - 10.;
		sprintf(snode_str, " %+.1f", fl);
	}
	return snode_str;
}

//
// Slinf: returns linear float [min..max] in different formats
char* Slinf(float fin, float fmin, float fmax, int pre) {
	char formt[8] = " %.0f";

	formt[3] = (char) (48 + pre); // results in " %.0f"... " %.3f" for pre = 0..3
	sprintf(snode_str, formt, fmin + (fmax - fmin) * fin);
	return snode_str;
}

//
// Slinfs: returns linear float [min..max] in different signed formats
char* Slinfs(float fin, float fmin, float fmax, int pre) {
	char formt[8] = " %+.0f";

	formt[4] = (char) (48 + pre); // results in " %+.0f"... " %+.3f" for pre = 0..3
	sprintf(snode_str, formt, fmin + (fmax - fmin) * fin);
	return snode_str;
}

//
// Slogf: returns log float [min..max] in different formats
char* Slogf(float fin, float fmin, float fmax, int pre) {
	char formt[8] = " %.0f";

	formt[3] = (char) (48 + pre); // results in " %.0f"... " %.3f" for pre = 0..3
	sprintf(snode_str, formt, exp(fin * log(fmax / fmin) + log(fmin)));
	return snode_str;
}

//
// Sbitmp: returns bitmap %chain from int
char* Sbitmp(int iin, int len) {
	int i, j;

	j = 0;
	snode_str[j++] = ' ';
	snode_str[j++] = '%';
	for (i = len - 1; i > -1; i--) {
		snode_str[j++] = ((iin & (1 << i)) ? '1' : '0');
	}
	snode_str[j] = 0;
	return snode_str;
}

//
// Sint: returns int as string
char* Sint(int iin) {
	sprintf(snode_str, " %d", iin);
	return snode_str;
}

//
// RLinf: reads linear float [min..max]
char* RLinf(X32command* command, char* str_pt_in, float xmin, float lmaxmin) {

	float fval;
	int len = 0;
	// calculate length of parameter
	if (*str_pt_in == '\0') return (NULL);
	while ((str_pt_in[len] != ' ') && (str_pt_in[len] != '\0')) len++;
	fval = Xr_float(str_pt_in, len);
//	fout = (fin - xmin) / (xmax-xmin)
	fval = (fval - xmin) / lmaxmin;
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	if ((fval < command->value.ff - EPSILON) || (fval > command->value.ff + EPSILON)) {
//	if (fval != command->value.ff) {
		command->value.ff = fval;
		s_len = Xfprint(s_buf, 0, command->command, 'f', &fval);
		Xsend(S_REM); // update xremote clients
	}
	str_pt_in += len;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
// RLogf: reads logarithm float [min..max]
char* RLogf(X32command* command, char* str_pt_in, float xmin, float lmaxmin) {

	float fval;
	int len = 0;
	// calculate length of parameter
	if (*str_pt_in == '\0') return (NULL);
	while ((str_pt_in[len] != ' ') && (str_pt_in[len] != '\0')) len++;
	fval = Xr_float(str_pt_in, len);
	fval = log(fval / xmin) / lmaxmin; // lmaxmin = log(xmax / xmin)
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	if ((fval < command->value.ff - EPSILON) || (fval > command->value.ff + EPSILON)) {
//	if (fval != command->value.ff) {
		command->value.ff = fval;
		s_len = Xfprint(s_buf, 0, command->command, 'f', &fval);
		Xsend(S_REM); // update xremote clients
	}
	str_pt_in += len;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
// REnum: sets int value from choice in list of strings separated by spaces
char* REnum(X32command* command, char* str_pt_in, char* str_enum[]) {
	int i, l_in;
	char csave;

	i = l_in = 0;
	if (*str_pt_in == '\0') return (NULL);
	while (str_pt_in[l_in] != ' ') l_in++;
	csave = str_pt_in[l_in];
	str_pt_in[l_in] = 0;
	while (*str_enum[i]) {
		if (strcmp(str_pt_in, str_enum[i]) == 0) {
			if (i == command->value.ii) {
				command->value.ii = i;
				s_len = Xfprint(s_buf, 0, command->command, 'i', &i);
				Xsend(S_REM); // update xremote clients
			}
			break;
		}
		i++;
	}
	str_pt_in[l_in] = csave;
	while ((*str_pt_in != ' ') && (*str_pt_in != '\0')) str_pt_in++;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
// SetFxPar1: set FX data from
void SetFxPar1(X32command* command, char* str_pt_in, int ipar, int type) {
	switch (type) {
	case _1_HALL:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.2, 3.218875825)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 98.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 29.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 1.386294361)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 250.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_AMBI:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.2, 3.597312261)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 198.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 29.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_RPLT:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.3, 4.571268634)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 4., 35.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.25, -6.684611728)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 1200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 1200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		break;
	case _1_ROOM:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.3, 4.571268634)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 4., 72.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.25, -6.684611728)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 250.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 1200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 1200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		break;
	case _1_CHAM:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.3, 4.571268634)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 4., 68.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.25, -6.684611728)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 250.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 500.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 500.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		break;
	case _1_PLAT:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 98.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 29.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 1.386294361)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10.,3.912023005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 49.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_VREV:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 120.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0.4, 4.1)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R01)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10000., 9.210340372)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 1.386294361)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.25, 1.386294361)) == NULL) return;
		break;
	case _1_VRM:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 5.298317367)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 98.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 29.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_GATE:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 140., 860.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 30.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 49.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -30., 30.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 29.)) == NULL) return;
		break;
	case _1_RVRS:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 140., 1.966112856)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 29.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -30., 30.)) == NULL) return;
		break;
	case _1_DLY:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R24)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 99.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 99.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		break;
	case _1_3TAP:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_4TAP:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 6.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R25)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_CRS:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 3.688879454)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 3.688879454)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_FLNG:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200.,4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -90., 180.)) == NULL) return;
		break;
	case _1_PHAS:
	case _2_PHAS:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 80.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1., 7.60090246)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 4.605170186)) == NULL) return;
		break;
	case _1_DIMC:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R24)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_FILT:
	case _2_FILT:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 5.991464547)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 20., 6.620073207)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R23)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R22)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.218875825)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R26)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_ROTA:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 3.688879454)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 2., 1.609437912)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R27)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R28)) == NULL) return;
		break;
	case _1_PAN:
	case _2_PAN:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1., 7.60090246)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 4.605170186)) == NULL) return;
		break;
	case _1_SUB:
	case _2_SUB:
		for (int j = 0; j < 2; j++) {
			if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
			if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R21)) == NULL) return;
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		}
		break;
	case _1_D_RV:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R20)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 3.912923005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 98.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912923005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_CR_R:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.382026635)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 3.912923005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 98.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912923005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_FL_R:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.382026635)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 3.688879454)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -90., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.1, 3.912923005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 2., 98.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912923005)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_D_CR:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R20)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.382026635)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_D_FL:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R20)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 4.382026635)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.5, 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -90., 180.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_MODD:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 2999.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R19)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 10., 3.912023005)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 200., 4.605170186)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 5.298317367)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R18)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R17)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 9.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.995732274)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	case _1_GEQ2:
	case _1_TEQ2:
	case _2_GEQ2:
	case _2_TEQ2:
		for (int j = 0; j < 64; j++) {
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -15., 30.)) == NULL) return;
		}
		break;
	case _1_GEQ:
	case _1_TEQ:
	case _2_GEQ:
	case _2_TEQ:
		for (int j = 0; j < 32; j++) {
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -15., 30.)) == NULL) return;
		}
		break;
	case _1_DES2:
	case _2_DES2:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R16)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R16)) == NULL) return;
		break;
	case _1_DES:
	case _2_DES:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R16)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R29)) == NULL) return;
		break;
	case _1_P1A2:
	case _2_P1A2:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R15)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R14)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R13)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
	case _1_P1A:
	case _2_P1A:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R15)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R14)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R13)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_PQ5S:
	case _2_PQ5S:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R12)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R11)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R10)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
	case _1_PQ5:
	case _2_PQ5:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R12)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R11)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R10)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_WAVD:
	case _2_WAVD:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		break;
	case _1_LIM:
	case _2_LIM:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 18.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -18., 18.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 0.05, 2.995732274)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 20., 4.605170186)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_CMB2:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R07)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 19.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 20., 5.010635294)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R08)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R09)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -40., 40.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R05)) == NULL) return;
	case _1_CMB:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R07)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 19.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 20., 5.010635294)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R08)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R09)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -40., 40.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -10., 20.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R06)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R05)) == NULL) return;
		break;
	case _1_FAC2:
	case _1_FAC1M:
	case _2_FAC2:
	case _2_FAC1M:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -20., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 7.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -18., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -20., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 7.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -18., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		break;
	case _1_FAC:
	case _2_FAC:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -20., 20.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 7.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -18., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		break;
	case _1_LEC2:
	case _2_LEC2:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R04)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -18., 24.)) == NULL) return;
	case _1_LEC:
	case _2_LEC:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R04)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -18., 24.)) == NULL) return;
		break;
	case _1_ULC2:
	case _2_ULC2:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -48., 0.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -48., 0.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 6.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 6.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R03)) == NULL) return;
	case _1_ULC:
	case _2_ULC:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -48., 0.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -48., 0.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 6.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 1., 6.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R03)) == NULL) return;
		break;
	case _1_ENH2:
	case _2_ENH2:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_ENH:
	case _2_ENH:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_EXC2:
	case _2_EXC2:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.302585093)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
	case _1_EXC:
	case _2_EXC:
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.302585093)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_IMG:
	case _2_IMG:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 12.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 100., 2.302585093)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1., 2.302585093)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		break;
	case _1_EDI:
	case _2_EDI:
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R02)) == NULL) return;
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R02)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		break;
	case _1_SON:
	case _2_SON:
		for (int j = 0; j < 2; j++) {
			if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		}
		break;
	case _1_AMP2:
	case _2_AMP2:
		for (int j = 0; j < 8; j++) {
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		}
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
	case _1_AMP:
	case _2_AMP:
		for (int j = 0; j < 8; j++) {
			if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 10.)) == NULL) return;
		}
		if ((str_pt_in = REnum(&command[ipar++], str_pt_in, R00)) == NULL) return;
		break;
	case _1_DRV2:
	case _2_DRV2:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 20., 2.302585093)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 4000., 1.609437912)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 50., 2.079441542)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.302585093)) == NULL) return;
	case _1_DRV:
	case _2_DRV:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 50.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 20., 2.302585093)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 4000., 1.609437912)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 50., 2.079441542)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1000., 2.302585093)) == NULL) return;
		break;
	case _1_PIT2:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1., 6.214608098)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1., 6.214608098)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 2000., 2.302585093)) == NULL) return;
		break;
		case _1_PIT:
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -12., 24.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -50., 100.)) == NULL) return;
		if ((str_pt_in = RLogf(&command[ipar++], str_pt_in, 1., 6.214608098)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, -100., 200.)) == NULL) return;
		if ((str_pt_in = RLinf(&command[ipar++], str_pt_in, 0., 100.)) == NULL) return;
		break;
	}
}
//
// GetFxPar1: concatenates FX data in buf
void GetFxPar1(X32command* command, char* buf, int ipar, int type) {
	int i;

	switch (type) {
	case _1_HALL:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.2, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 2., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 250., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_AMBI:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.2, 7.3, 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_RPLT:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.3, 29., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 4., 39., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.25, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		break;
	case _1_ROOM:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.3, 29., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 4., 76., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.25, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 250., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 1200., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		break;
	case _1_CHAM:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.3, 29., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 4., 72., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.25, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 250., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 500., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 500., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_PLAT:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 10., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 2., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_VREV:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 120., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0.4, 4.5, 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " REAR" : " FRONT");
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10000., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 2., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.25, 1., 0));
		break;
	case _1_VRM:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 20., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 10., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 10., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 1));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_GATE:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 140, 1000., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 30., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -30., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 30., 0));
		break;
	case _1_RVRS:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 140, 1000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 30., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., +12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -30., 0., 0));
		break;
	case _1_DLY:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Smode[command[ipar++].value.ii]);
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, -100., +100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		break;
	case _1_3TAP:
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_4TAP:
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200, 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 6., 0));
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Sfactor[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_CRS:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 50., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 50., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_FLNG:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 200., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -90., 90., 0));
		break;
	case _1_PHAS:
	case _2_PHAS:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 80., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 1000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 1., 2000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 1000., 1));
		break;
	case _1_DIMC:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ST" : " M");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_FILT:
	case _2_FILT:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 20., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 20., 15000., 1));
		strcat(buf, Sfmode[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Sfwave[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 250., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 2POL" : " 4POL");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_ROTA:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 4., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 2., 10., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " RUN" : " STOP");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " SLOW" : " FAST");
		break;
	case _1_PAN:
	case _2_PAN:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 1000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 1., 2000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 1000., 1));
		break;
	case _1_SUB:
	case _2_SUB:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Sfrange[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Sfrange[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_D_RV:
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfpattern[command[ipar++].value.ii]);
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000, 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10, 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_CR_R:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 50., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_FL_R:
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -90., 90., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 200., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.1, 5., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 2., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_D_CR:
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfpattern[command[ipar++].value.ii]);
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 50., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_D_FL:
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfpattern[command[ipar++].value.ii]);
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 4., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.5, 20., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 180., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -90., 90., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_MODD:
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 3000., 0));
		strcat(buf, Sfddelay[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 10., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 10., 1));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " SER" : " PAR");
		strcat(buf, Sfdtype[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 10., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	case _1_GEQ2:
	case _1_TEQ2:
	case _2_GEQ2:
	case _2_TEQ2:
		for (i = 0; i < 64; i++) {
			strcat(buf, Slinf(command[ipar++].value.ff, -15., 15., 0));
		}
		break;
	case _1_GEQ:
	case _1_TEQ:
	case _2_GEQ:
	case _2_TEQ:
		for (i = 0; i < 32; i++) {
			strcat(buf, Slinf(command[ipar++].value.ff, -15., 15., 0));
		}
		break;
	case _1_DES2:
	case _2_DES2:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " MALE" : " FEM");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " MALE" : " FEM");
		break;
	case _1_DES:
	case _2_DES:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " MALE" : " FEM");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " MS" : " ST");
		break;
	case _1_P1A2:
	case _2_P1A2:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfplfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfpmfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfphfreq[command[ipar++].value.ii]);
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
	case _1_P1A:
	case _2_P1A:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfplfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfpmfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfphfreq[command[ipar++].value.ii]);
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_PQ5S:
	case _2_PQ5S:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Sfqlfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqmfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqhfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
	case _1_PQ5:
	case _2_PQ5:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Sfqlfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqmfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Sfqhfreq[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_WAVD:
	case _2_WAVD:
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -24., 24., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -24., 24., 0));
		break;
	case _1_LIM:
	case _2_LIM:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -18., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 0.05, 1., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 20., 2000., 1));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_CMB2:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Sflcmb[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 19., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 20., 3000., 1));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 48" : " 12");
		strcat(buf, Sfrcmb[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, -40., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Sfmcmb[command[ipar++].value.ii]);
	case _1_CMB:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Sflcmb[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 19., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 20., 3000., 1));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 48" : " 12");
		strcat(buf, Sfrcmb[command[ipar++].value.ii]);
		strcat(buf, Slinf(command[ipar++].value.ff, -40., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -10., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " 1" : " 0");
		strcat(buf, Sfmcmb[command[ipar++].value.ii]);
		break;
	case _1_FAC2:
	case _1_FAC1M:
	case _2_FAC2:
	case _2_FAC1M:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -20., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 6., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -18., 6., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -20., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 6., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -18., 6., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		break;
	case _1_FAC:
	case _2_FAC:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -20., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 6., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -18., 6., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		break;
	case _1_LEC2:
	case _2_LEC2:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " LIM" : " COMP");
		strcat(buf, Slinf(command[ipar++].value.ff, -18., 6., 0));
	case _1_LEC:
	case _2_LEC:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " LIM" : " COMP");
		strcat(buf, Slinf(command[ipar++].value.ff, -18., 6., 0));
		break;
	case _1_ULC2:
	case _2_ULC2:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Sfrulc[command[ipar++].value.ii]);
	case _1_ULC:
	case _2_ULC:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -48., 0., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 7., 0));
		strcat(buf, Sfrulc[command[ipar++].value.ii]);
		break;
	case _1_ENH2:
	case _2_ENH2:
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_ENH:
	case _2_ENH:
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 1., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_EXC2:
	case _2_EXC2:
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 10000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
	case _1_EXC:
	case _2_EXC:
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 10000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_IMG:
	case _2_IMG:
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 100., 1000., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 1., 10., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		break;
	case _1_EDI:
	case _2_EDI:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " M/S" : " ST");
		strcat(s_buf + s_len, command[ipar++].value.ii ? " M/S" : " ST");
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		break;
	case _1_SON:
	case _2_SON:
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		break;
	case _1_AMP2:
	case _2_AMP2:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
	case _1_AMP:
	case _2_AMP:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 10., 0));
		strcat(s_buf + s_len, command[ipar++].value.ii ? " ON" : " OFF");
		break;
	case _1_DRV2:
	case _2_DRV2:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 20., 200., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 4000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 50., 400., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 10000., 1));
	case _1_DRV:
	case _2_DRV:
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 50., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 20., 200., 1));
		strcat(buf, Slogf(command[ipar++].value.ff, 4000., 20000., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 50., 400., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1000., 10000., 1));
		break;
	case _1_PIT2:
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 2000., 20000., 1));
		break;
	case _1_PIT:
		strcat(buf, Slinf(command[ipar++].value.ff, -12., 12., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -50., 50., 0));
		strcat(buf, Slogf(command[ipar++].value.ff, 1., 500., 1));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, -100., 100., 0));
		strcat(buf, Slinf(command[ipar++].value.ff, 0., 100., 0));
		break;
	default:
		break;
	}
	return;
}
//
// funct_params: parse commands data
int funct_params(X32command *command, int i) {
	int j, c_len, f_len, f_num, c_type, update;
	char* s_adr;
	char* s_fmt;
	char loc_str[64];
//
// warning... This function will correctly parse and accept /ch/xx/config ,siii <name> [i] [i] [i]
// when the X32 will not accept it
// The XAIR series accept the function and option correctly; X32 & M32 do not as
// a node address followed by a string is interpreted differently by the X32-edit application
// (so says Behringer).
	// save command index for possible function_node_single() use
	node_single_command = command;
	node_single_index = i;
	//
	f_len = f_num = c_type = update =0;
	c_len = strlen(command[i].command);
	f_len = (((c_len + 4) & ~3) + 1); // pointing at first format char after ',' if there's a ','
	if ((r_len - 4 > c_len) && (r_buf[f_len] != 0)) { // there's a ',' and at least one type tag
		// First of a list command gives the first of next data
		if (command[i].flags & F_FND) ++i;
		// command has parameter(s) (set)
		if (command[i].flags & F_SET) {
			c_len = f_len; // now pointing at first format char after ','
			f_num = 0;
			while (r_buf[c_len++]) ++f_num; // count number of type tag characters
			c_len = (c_len + 3) & ~3; // now pointing at first argument value
			while (f_num--) {
				switch (r_buf[f_len++]) {
				case 'i':
					j = 4;
					while (j) endian.cc[--j] = r_buf[c_len++];
					//save value to respective field - index i
					if (command[i].flags & F_SET) {
						if (command[i].value.ii != endian.ii) {
							update = 1;
							command[i].value.ii = endian.ii;
						}
					}
					break;
				case 'f':
					j = 4;
					while (j) endian.cc[--j] = r_buf[c_len++];
					//save value to respective field - index i
					if (command[i].flags & F_SET) {
						if (command[i].value.ff != endian.ff) {
							update = 1;
							command[i].value.ff = endian.ff;
						}
					}
					break;
				case 's':
					j = strlen(r_buf + c_len); // actual need can be up to 4 more \0 bytes; add 8 by security
					strncpy(loc_str, r_buf + c_len, j);
					loc_str[j] = 0;
					if (command[i].flags & F_SET) {
						if (j > 0) {
							if (command[i].value.str) update = strcmp(command[i].value.str, loc_str);
							else                      update = 1;
							if (command[i].value.str) free(command[i].value.str);
							command[i].value.str = malloc((j + 8) * sizeof(char));
							strcpy(command[i].value.str, loc_str);
						} else {
							if (command[i].value.str) {
								free(command[i].value.str);
								command[i].value.str = NULL;
								update = 1;
							}
						}
					}
					c_len = ((c_len + j + 4) & ~3);
					break;
				case 'b':
					printf("parameter: blob\n");
					break;
				}
				i += 1;
			}
			if (update) {
				memcpy(s_buf, r_buf, r_len);
				s_len = r_len;
				// no need for sending to local client
				// authorize remote clients to receive info
				p_status = S_REM;
			} else p_status = 0;
		} // done parsing
	} else {
		// First of a list command gives the first of next data
		if (command[i].flags & F_FND) ++i;
		if (command[i].flags & F_GET) { // if the command is part of the GET family
			// command without parameters: (get)
			s_len = Xsprint(s_buf, 0, 's', command[i].command);
			c_type = command[i].format.typ;
			if (c_type == FX32) {
				// special case of FX parameters. Need to validate if the requested parameter
				// is an int of a float. Decision based on a lookup table. Once found,
				// we process normally
				c_type = FXc_lookup(command, i); // the function returns I32, F32, S32,...
			}
			if (c_type == I32 || c_type == E32 || c_type == P32) {
				s_len = Xsprint(s_buf, s_len, 's', ",i");
				s_len = Xsprint(s_buf, s_len, 'i', &command[i].value.ii);
			} else if (c_type == F32) {
				s_len = Xsprint(s_buf, s_len, 's', ",f");
				s_len = Xsprint(s_buf, s_len, 'i', &command[i].value.ff);
			} else if (c_type == S32) {
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				if (command[i].value.str) s_len = Xsprint(s_buf, s_len, 's', command[i].value.str);
				else s_len = Xsprint(s_buf, s_len, 's', &zero); // return nil chars if no string
			} else if (c_type == B32) {
				if ((s_adr = command[i].value.dta) != NULL) {
					s_len = Xsprint(s_buf, s_len, 's', ",b"); //todo - incorrect
					s_len = Xsprint(s_buf, s_len, 'b', command[i].value.dta);
				}
			} else {
				if ((s_adr = command[i].value.str) != NULL) {
					s_len = Xsprint(s_buf, s_len, 's', command[i].format.str);
					s_fmt = command[i].format.str + 1;
					while (*s_fmt) {
						if (*s_fmt == 'i') {
							s_len = Xsprint(s_buf, s_len, 'i', (int*) s_adr);
							s_adr += 4;
						} else if (*s_fmt == 'f') {
							s_len = Xsprint(s_buf, s_len, 'f', (float*) s_adr);
							s_adr += 4;
						} else if (*s_fmt == 's') {
							if (s_adr)
								s_len = Xsprint(s_buf, s_len, 's', s_adr);
							s_adr += ((strlen(s_adr) + 4) & ~3);
						} else if (*s_fmt == 'b') {
							s_len = Xsprint(s_buf, s_len, 'b', (void*) s_adr);
							s_adr += ((strlen(s_adr) + 4) & ~3); //todo - incorrect
						}
						s_fmt++;
					}
				}
			}
			// need to send to local client
			// no need to send to remote clients
			p_status = S_SND;
		}
	}
	return (p_status);
}

//
// no-op functions
int function() {
	printf("dummy function\n");
	fflush(stdout);
	return 0;
}

//
// /info command
int function_info() {
	s_len = Xsprint(s_buf, 0, 's', "/info");
	s_len = Xsprint(s_buf, s_len, 's', ",ssss");
	s_len = Xsprint(s_buf, s_len, 's', "V2.07");
	if (Xprefs[X32NAME].value.str) s_len = Xsprint(s_buf, s_len, 's', Xprefs[X32NAME].value.str);
	else                           s_len = Xsprint(s_buf, s_len, 's', "X32 Emulator");
	s_len = Xsprint(s_buf, s_len, 's', "X32");
	s_len = Xsprint(s_buf, s_len, 's', XVERSION);
	return S_SND; // send reply only to requesting client
}

//
// /xinfo command
int function_xinfo() {
	s_len = Xsprint(s_buf, 0, 's', "/xinfo");
	s_len = Xsprint(s_buf, s_len, 's', ",ssss");
	s_len = Xsprint(s_buf, s_len, 's', Xip_str);
	if (Xprefs[X32NAME].value.str) s_len = Xsprint(s_buf, s_len, 's', Xprefs[X32NAME].value.str);
	else                           s_len = Xsprint(s_buf, s_len, 's', "X32 Emulator");
	s_len = Xsprint(s_buf, s_len, 's', "X32");
	s_len = Xsprint(s_buf, s_len, 's', XVERSION);
	return S_SND; // send reply only to requesting client
}

//
// /status command
int function_status() {
	// TODO getmyIP(); // get my IP in r_buf

	s_len = Xsprint(s_buf, 0, 's', "/status");
	s_len = Xsprint(s_buf, s_len, 's', ",sss");
	s_len = Xsprint(s_buf, s_len, 's', "active");
	s_len = Xsprint(s_buf, s_len, 's', Xip_str);
	if (Xprefs[X32NAME].value.str) s_len = Xsprint(s_buf, s_len, 's', Xprefs[X32NAME].value.str);
	else                           s_len = Xsprint(s_buf, s_len, 's', "X32 Emulator");
	return S_SND; // send reply only to requesting client
}

//
// /unsubscribe command
int function_unsubscribe() {
	int k;
	//
	// For now, only simple/single unsubscribe command is recognized to remove
	// possible xremote client
	for (k = 0; k < MAX_CLIENTS; k++) {
		if (X32Client[k].vlid) {
			if (strcmp(X32Client[k].sock.sa_data, Client_ip_pt->sa_data) == 0) {
				X32Client[k].vlid = 0; //No longer a valid client
				return 0;
			}
		}
	}
	return 0;
}

//
// /xremote command
int function_xremote() {
	int k;
	//
	// command is xremote (set remote time for requesting client)
	// Need to update existing client or register requesting client as new client?
	for (k = 0; k < MAX_CLIENTS; k++) {
		if (X32Client[k].vlid) {
			if (strcmp(X32Client[k].sock.sa_data, Client_ip_pt->sa_data) == 0) {
				X32Client[k].xrem = time(NULL) + XREMOTE_TIME; //update existing client
				return 0;
			}
		}
	}
	// attempt to register a new client... if room available
	for (k = 0; k < MAX_CLIENTS; k++) {
		if (X32Client[k].vlid == 0) { // create new record
//			memcpy(&(X32Client[k].sock), Client_ip_pt, Client_ip_len);
			X32Client[k].sock = *Client_ip_pt;
			X32Client[k].vlid = 1;
			X32Client[k].xrem = time(NULL) + XREMOTE_TIME;
			return 0;
		} else if (X32Client[k].xrem < time(NULL)) { // replace outdated record
//			memcpy(&(X32Client[k].sock), Client_ip_pt, Client_ip_len);
			X32Client[k].sock = *Client_ip_pt;
			X32Client[k].xrem = time(NULL) + XREMOTE_TIME;
			return 0;
		}
	}
	return 0; // no room for new clients! (todo; another return status?)
}
//
//
char* XslashSetInt(X32command* command, char* str_pt_in) {
	int i = 0;

	if (*str_pt_in == '\0') return (NULL);
	sscanf(str_pt_in, "%d", &i);
	if (i != command->value.ii) {
		command->value.ii = i;
		s_len = Xfprint(s_buf, 0, command->command, 'i', &i);
		Xsend(S_REM); // update xremote clients
	}
	while ((*str_pt_in != ' ') && (*str_pt_in != '\0')) str_pt_in++;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
//
char* XslashSetPerInt(X32command* command, char* str_pt_in) {

int i, j;
	// to be set in subroutines: int (int)
	j = 0;
	if (*str_pt_in == '\0') return (NULL);
	if (*str_pt_in == '%') {
		// we expect the rest of the string to contain only 0 and 1 chars
		i = 1;
		while ((str_pt_in[i] != ' ') && (str_pt_in[i] != '\0')) {
			command->value.ii <<= 1;
			if (str_pt_in[i] == '1') j |= 1;
			i++;
		}
	} else {
		sscanf(str_pt_in, "%d", &j);
	}
	if (j != command->value.ii) {
		command->value.ii = j;
		s_len = Xfprint(s_buf, 0, command->command, 'i', &j);
		Xsend(S_REM); // update xremote clients
	}
	while ((*str_pt_in != ' ') && (*str_pt_in != '\0')) str_pt_in++;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
//
char* XslashSetString(X32command* command, char* str_pt_in) {
	char* str_pt;
	char loc_str[64];
	int j, cmore, update;
	//
	if (*str_pt_in == '\0') return (NULL);
	while (*str_pt_in == ' ') str_pt_in++;
	// search for end of string (either ' ' or ")
	if (*str_pt_in == '"') {
		cmore = 1;
		str_pt_in++;
		str_pt = str_pt_in;
		while (*str_pt != '"') str_pt++;
	} else {
		cmore = 0;
		str_pt = str_pt_in;
		while ((*str_pt != ' ') && (*str_pt != '\n')) str_pt++;
	}
	update = 0;
	j = str_pt - str_pt_in;
	strncpy(loc_str, str_pt_in, j);
	loc_str[j] = 0;
	if (j > 0) {
		if (command->value.str) update = strcmp(command->value.str, loc_str);
		else                    update = 1;
		if (command->value.str) free(command->value.str);
		command->value.str = malloc((j + 8) * sizeof(char));
		strcpy(command->value.str, loc_str);
	} else {
		if (command->value.str) {
			free(command->value.str);
			command->value.str = NULL;
			update = 1;
		}
	}
	if (update) {
		s_len = Xfprint(s_buf, 0, command->command, 's', command->value.str);
		Xsend(S_REM); // update xremote clients
	}
	if (cmore) str_pt++;
	while (*str_pt == ' ') str_pt++;
	return str_pt;
}
//
//
char* XslashSetLevl(X32command* command, char* str_pt_in, int nsteps) {
	float fval;
	int len = 0;
	// calculate length of parameter
	if (*str_pt_in == '\0') return (NULL);
	while ((str_pt_in[len] != ' ') && (str_pt_in[len] != '\0'))	len++;
	if (str_pt_in[0] == '-' && str_pt_in[1] == 'o' && str_pt_in[2] == 'o') fval = 0.0;
	else {
		sscanf(str_pt_in, "%f", &fval);
		if (fval < -60.) {
// first slope, make sure we don't generate negative values
//			if ((fval = 0.0625 / 30. * (fval + 90.)) < 0.0) fval = 0.0;
			fval = fval * 0.00208333333 + 0.1875;
			fval = (int)(fval * (nsteps + 0.5)) / (float)nsteps;
			if (fval < 0.0) fval = 0.0;
		} else if (fval < -30.) {
// second slope
//			fval = 0.0625 + (0.25 - 0.0625) / 30. * (fval + 60.);
			fval = 0.00625 * fval + 0.4375;
			fval = (int)(fval * (nsteps + 0.5)) / (float)nsteps;
		} else if (fval < -10.) {
// third slope
//			fval = 0.25 + 0.25 / 20. * (fval + 30.);
			fval = 0.0125 * fval + 0.625;
			fval = (int)(fval * (nsteps + 0.5)) / (float)nsteps;
		} else if (fval <= 10.) {
// fourth and high values slope; make sure we don't go over 1.0
//			if ((fval = 0.5 + 0.5 / 20. * (fval + 10.)) > 1.0) fval = 1.0;
			fval = fval * 0.025 + 0.75;
			if ((fval = (int)(fval * (nsteps + 0.5)) / (float)nsteps) > 1.0) fval = 1.0;
		} else if (fval > 10.) fval = 1.0;
	}
	if ((fval < command->value.ff - EPSILON) || (fval > command->value.ff + EPSILON)) {
//	if (fval != command->value.ff) {
		command->value.ff = fval;
		s_len = Xfprint(s_buf, 0, command->command, 'f', &fval);
		Xsend(S_REM); // update xremote clients
	}
	str_pt_in += len;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
//
char* XslashSetList(X32command* command, char* str_pt_in) {
	int j = 0;
	int len = 0;
	char csave;
	// calculate length of parameter
	if (*str_pt_in == '\0') return (NULL);
	while ((str_pt_in[len] != ' ') && (str_pt_in[len] != '\0'))	len++;
	csave = str_pt_in[len];
	str_pt_in[len] = 0;
	if (command->node) {
		while (*command->node[j]) {
			if (strcmp(command->node[j]+1, str_pt_in) == 0) {
				if (j != command->value.ii) {
					command->value.ii = j;
					s_len = Xfprint(s_buf, 0, command->command, 'i', &j);
					Xsend(S_REM); // update xremote clients
				}
				break;
			}
			j++;
		}
	}
	str_pt_in[len] = csave;
	str_pt_in += len;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}

//
//
float Xr_float(char* Xin, int l) {
char llread[16]; // max length for float argument when read as a string
float fval;
int i, ival, idec;
//
// float number, in the form nnn, nnn.ff, or nnnkff
// let's read data and search for punctuation ('.' or 'k')
	strncpy(llread, Xin, l);
	llread[l] = 0;
	for (i = 0; i < l; i++) {
		if (llread[i] == '.') {
			sscanf(llread, "%f", &fval);
			return (fval);
		} else if (llread[i] == 'k') {
			ival = 0; idec = 0;
			llread[i] = 0;
			if (i > 0) sscanf(llread, "%d", &ival);
			if (i < l) sscanf(llread + i + 1, "%d", &idec);
			fval = (float)ival * 1000.;
			if (l-i == 2) fval += (float)idec * 100.;
			else if (l-i == 3) fval += (float)idec * 10.;
			else if (l-i == 4) fval += (float)idec;
			return (fval);
		}
	}
//no punctuation mark case
	sscanf(llread, "%f", &fval);
	return (fval);
}
//
//
char* XslashSetLogf(X32command* command, char* str_pt_in, float xmin, float lmaxmin, int nsteps) {
	int len = 0;
	float fval;
	// calculate length of parameter
	if (*str_pt_in == '\0') return (NULL);
	while ((str_pt_in[len] != ' ') && (str_pt_in[len] != '\0')) len++;
	fval = Xr_float(str_pt_in, len);

	fval = log(fval / xmin) / lmaxmin; // lmaxmin = log(xmax / xmin)
// round to nsteps' value of log()
	fval = roundf(fval * nsteps) / nsteps;
//	if (lmaxmin > 0.) fval = roundf(fval * nsteps) / nsteps;
//	else              fval = floorf(fval * nsteps) / nsteps;
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	if ((fval < command->value.ff - EPSILON) || (fval > command->value.ff + EPSILON)) {
//	if (fval != command->value.ff) {
		command->value.ff = fval;
		s_len = Xfprint(s_buf, 0, command->command, 'f', &fval);
		Xsend(S_REM); // update xremote clients
	}
	while ((*str_pt_in != ' ') && (*str_pt_in != '\0')) str_pt_in++;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
//
char* XslashSetLinf(X32command* command, char* str_pt_in, float xmin, float lmaxmin, float xstep) {
	float fval;
	int len = 0;
	// calculate length of parameter
	if (*str_pt_in == '\0') return (NULL);
	while ((str_pt_in[len] != ' ') && (str_pt_in[len] != '\0')) len++;
	fval = Xr_float(str_pt_in, len);
//	fout = (fin - xmin) / (xmax-xmin)
	fval = (fval - xmin) / lmaxmin;
	// round to xstep value
	xstep = lmaxmin/xstep;
	//	fval = ceilf(fval*xstep) / xstep;
	fval = roundf(fval*xstep) / xstep;
	if (fval <= 0.) fval = 0.; // avoid -0.0 values (0x80000000)
	if (fval > 1.) fval = 1.;
	if ((fval < command->value.ff - EPSILON) || (fval > command->value.ff + EPSILON)) {
//	if (fval != command->value.ff) {
		command->value.ff = fval;
		s_len = Xfprint(s_buf, 0, command->command, 'f', &fval);
		Xsend(S_REM); // update xremote clients
	}
	str_pt_in += len;
	while (*str_pt_in == ' ') str_pt_in++;
	return str_pt_in;
}
//
// reply to '/' commands
int function_slash() {
	char w_buf[BSIZE];
	int  w_len;
	char* str_pt_in;
	int i, j, n, cmd_max, c_len, c_type;
	X32command* command;
	// received a /~~~,s~~[string] [[data]...]
	// parse [string]
	// set internal variable according to [[data]...] contents
	// return the command we received to sender
	//
	// prepare data to be sent back
	w_len = r_len;
	memcpy(w_buf, r_buf, r_len);
	{
		// Main work goes here
		cmd_max = 0;
		str_pt_in = r_buf + 8;				// data block starts at index 8
		if (*str_pt_in == '/') str_pt_in++;
		for (n = 0; n < Xnode_max; n++) {
			if (strncmp(Xnode[n].command, str_pt_in, Xnode[n].nchars) == 0) {
				cmd_max = Xnode[n].cmd_max;
				command = Xnode[n].cmd_ptr;
				break;
			}
		}
		if (n < Xnode_max) {
			// search exact command in command node set
			// command stops at first space found
			i = str_pt_in - r_buf;
			c_len = 0;
			while (i < r_len) {
				if (r_buf[i] == ' ') break;
				c_len += 1;
				i += 1;
			}
			for (i = 0; i < cmd_max; i++) {
				if (strncmp(str_pt_in, command[i].command + 1, c_len) == 0) {
					str_pt_in += strlen(command[i].command +1) + 1; // point at [[data]...]
					// we now are at the right command, parse the alphanumeric data following the command
					// to set parameter values one by one
					if (command[i].flags == F_FND) {
						// skip successive F_FND types until i+1 ponts to a non F_FND (such as F_EXT)
						while (command[i+1].flags == F_FND) i++;
						// treat as variable length /node command. parsing data as needed
						switch (command[i].format.typ) {
						case CHCO:						// name, icon#, color, chan_input
							if ((str_pt_in = XslashSetString(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetInt(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+4], str_pt_in)) == NULL) return S_SND;
							break;
						case CHDE:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLinf(&command[i+2], str_pt_in, 0.3, 499.7, 0.1)) == NULL) return S_SND;
							break;
						case CHPR:
							if ((str_pt_in = XslashSetLinf(&command[i+1], str_pt_in, -18., 36., 0.25)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+5], str_pt_in, 20., 2.9957322735, 100)) == NULL) return S_SND; // log(400/20) = 2.9957322735
							break;
						case CHGA:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+3], str_pt_in, -80., 80., 0.5)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+4], str_pt_in, 3., 57., 1.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+5], str_pt_in, 0., 120., 1.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+6], str_pt_in, 0.02, 11.512925465, 100)) == NULL) return S_SND;	// log(2000/0.02) = 11.512925465
							if ((str_pt_in = XslashSetLogf(&command[i+7], str_pt_in, 5., 6.684611728, 100)) == NULL) return S_SND;		// log (4000/5) = 6.684611728
							if ((str_pt_in = XslashSetInt(&command[i+8], str_pt_in)) == NULL) return S_SND;
							break;
						case CHGF:
						case CHDF:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+3], str_pt_in, 20., 6.907755279, 200)) == NULL) return S_SND;	// log(20000/20) = 6.907755279
							break;
						case CHDY:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+5], str_pt_in, -60., 60., 0.5)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+6], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+7], str_pt_in, 0., 5.0, 1.0)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+8], str_pt_in, 0., 24.0, 0.5)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+9], str_pt_in, 0., 120.0, 1.0)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+10], str_pt_in, 0.02, 11.51292546, 100)) == NULL) return S_SND;	// log (2000/0.02) = 11.51292546
							if ((str_pt_in = XslashSetLogf(&command[i+11], str_pt_in, 5., 6.684611728, 100)) == NULL) return S_SND;		// log (4000/5) = 6.684611728
							if ((str_pt_in = XslashSetList(&command[i+12], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+13], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+14], str_pt_in, 0., 100.0, 5.0)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+15], str_pt_in)) == NULL) return S_SND;
							break;
						case CHIN:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							break;
						case CHEQ:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLogf(&command[i+2], str_pt_in, 20., 6.907755279, 200)) == NULL) return S_SND;	// log(20000/20) = 6.907755279
							if ((str_pt_in = XslashSetLinf(&command[i+3], str_pt_in, -15., 30.0, 0.250)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+4], str_pt_in, 10., -3.506557897, 71)) == NULL) return S_SND;	// log(0.3/10) = -3.506557897
							break;
						case CHMX:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLevl(&command[i+2], str_pt_in, 1023)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+4], str_pt_in, -100., 200., 2.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+5], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLevl(&command[i+6], str_pt_in, 160)) == NULL) return S_SND;
							break;
						case CHMO:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLevl(&command[i+2], str_pt_in, 1023)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+3], str_pt_in, -100., 200., 2.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+5], str_pt_in)) == NULL) return S_SND;
							break;
						case CHME:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLevl(&command[i+2], str_pt_in, 1023)) == NULL) return S_SND;
							break;
						case CHGRP:
							if ((str_pt_in = XslashSetPerInt(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetPerInt(&command[i+2], str_pt_in)) == NULL) return S_SND;
							break;
						case CHAMIX:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLinf(&command[i+2], str_pt_in, -12., 24., 0.5)) == NULL) return S_SND;
							break;
						case AXPR:						// trim, invert
							if ((str_pt_in = XslashSetLinf(&command[i+1], str_pt_in, -18., 36., 0.25)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return S_SND;
							break;
						case BSCO:						// name, icon#, color
							if ((str_pt_in = XslashSetString(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetInt(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							break;
						case MXPR:						// invert
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							break;
						case MXDY:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+5], str_pt_in, -60., 60., 0.5)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+6], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+7], str_pt_in, 0., 5.0, 1.0)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+8], str_pt_in, 0., 24.0, 0.5)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+9], str_pt_in, 0., 120.0, 1.0)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+10], str_pt_in, 0.02, 11.51292546, 100)) == NULL) return S_SND;	// log (2000/0.02) = 11.51292546
							if ((str_pt_in = XslashSetLogf(&command[i+11], str_pt_in, 5., 6.684611728, 100)) == NULL) return S_SND;		// log (4000/5) = 6.684611728
							if ((str_pt_in = XslashSetList(&command[i+12], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+13], str_pt_in, 0., 100.0, 5.0)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+14], str_pt_in)) == NULL) return S_SND;
							break;
						case MSMX:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLevl(&command[i+2], str_pt_in, 1023)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+3], str_pt_in, -100., 200., 2.)) == NULL) return S_SND;
							break;
						case FXTYP1:
						case FXTYP2:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							break;
						case FXSRC:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							break;
						case FXPAR1:
							SetFxPar1(command, str_pt_in, i + 1, command[i - 4].value.ii);
							break;
						case FXPAR2:
							SetFxPar1(command, str_pt_in, i + 1, command[i - 2].value.ii + _1_PIT + 2);
							break;
						case OMAIN:
							if ((str_pt_in = XslashSetInt(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							break;
						case OMAIN2:
							if ((str_pt_in = XslashSetInt(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							break;
						case OP16:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+4], str_pt_in)) == NULL) return S_SND;
							break;
						case OMAIND:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLinf(&command[i+2], str_pt_in, 0.3, 499.7, 0.1)) == NULL) return S_SND;
							break;
						case HAMP:
							if ((str_pt_in = XslashSetLinf(&command[i+1], str_pt_in, -12., 72., 0.5)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							break;
						case PREFS:
							if ((str_pt_in = XslashSetString(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLinf(&command[i+2], str_pt_in, 10., 90., 5.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+3], str_pt_in, 0., 100., 2.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+4], str_pt_in, 10., 90., 5.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLinf(&command[i+5], str_pt_in, 10., 90., 10.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+6], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+7], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+8], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+9], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+10], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+11], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+12], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+13], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+14], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+15], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetPerInt(&command[i+16], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+17], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+18], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+19], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+20], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+21], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+22], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetString(&command[i+23], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+24], str_pt_in)) == NULL) return S_SND;
							break;
						case PIR:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetPerInt(&command[i+4], str_pt_in)) == NULL) return S_SND;
							break;
						case PIQ:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+4], str_pt_in)) == NULL) return S_SND;
							break;
						case PCARD:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+5], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+6], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+7], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+8], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+9], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+10], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+11], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+12], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+13], str_pt_in)) == NULL) return S_SND;
							break;
						case PRTA:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetLinf(&command[i+2], str_pt_in, 0., 60., 6.)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+5], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+6], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetPerInt(&command[i+7], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+8], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetLogf(&command[i+9], str_pt_in, 0.25, 4.158883083, 19)) == NULL) return S_SND;	// log (16/0.25) = 4.158883083
							if ((str_pt_in = XslashSetList(&command[i+10], str_pt_in)) == NULL) return S_SND;
							break;
						case PIP:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							break;
						case PKEY:
							if ((str_pt_in = XslashSetInt(&command[i+1], str_pt_in)) == NULL) return 0;
							break;
						case PADDR:
						case PMASK:
						case PGWAY:
							if ((str_pt_in = XslashSetInt(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetInt(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+4], str_pt_in)) == NULL) return S_SND;
							break;
						case STAT:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetInt(&command[i+2], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+3], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+4], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+5], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+6], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+7], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+8], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+9], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+10], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+11], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+12], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+13], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+14], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+15], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+16], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+17], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+18], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+19], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+20], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+21], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+22], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetInt(&command[i+23], str_pt_in)) == NULL) return S_SND;
							break;
						case SSCREEN:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return S_SND;
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return S_SND;
							break;
						case SCHA:
						case SMET:
						case SROU:
						case SSET:
						case SLIB:
						case SFX:
						case SMON:
						case SUSB:
						case SSCE:
						case SASS:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							break;
						case SSOLOSW:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							for (j = 2; j < 81; j++) if ((str_pt_in = XslashSetList(&command[i+j], str_pt_in)) == NULL) return S_SND;
							break;
						case SOSC:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							break;
						case STALK:
							if ((str_pt_in = XslashSetList(&command[i+1], str_pt_in)) == NULL) return 0;
							if ((str_pt_in = XslashSetList(&command[i+2], str_pt_in)) == NULL) return S_SND;
							break;
//SAES,		// 76
//STAPE,	// 77
//USB,		// 80
//SNAM,		// 81
//SCUE,		// 82
//SSCN,		// 83
//SSNP,		// 84
//HA,		// 85
//ACTION,	// 86
//UREC		// 87
						}
						memcpy(s_buf, w_buf, w_len);
						s_len = w_len;
						return(S_SND); // send reply only to requesting client
					} else if (command[i].flags & F_SET) {
						if (*str_pt_in == '\0') return 0;
						while (*str_pt_in == ' ') str_pt_in ++;		// skip leading spaces
						c_type = command[i].format.typ;
						if (c_type == I32 || c_type == P32) {
							sscanf(str_pt_in, "%d", &endian.ii);
							command[i].value.ii = endian.ii;
						} else if (c_type == E32) {
							// data given as an alphanumerical enum; n point at the Xnode for the command
							XslashSetList(&command[i], str_pt_in);
						} else if (c_type == F32) {
							sscanf(str_pt_in, "%f", &endian.ff);
							command[i].value.ff = endian.ff;
						} else if (c_type == S32) {
							if (command[i].value.str) free(command[i].value.str);
							while (*str_pt_in == ' ') str_pt_in++;
							c_len = 0;
							if (*str_pt_in == '"') {
								str_pt_in++;
								c_len = 1;
							}
							j = strlen(str_pt_in) - c_len; // remove trailing " if there's one
							if (j > 0) {
								command[i].value.str = malloc((j + 8) * sizeof(char));
								strncpy(command[i].value.str, str_pt_in, j);
								command[i].value.str[j] = 0;
							} else {
								command[i].value.str = NULL;
							}
						} else {
							// Todo?
						}
						memcpy(s_buf, w_buf, w_len);
						s_len = w_len;
						return(S_SND); // send reply only to requesting client
					}
				}
			}
		}
	}
	return 0;
}
//
//
// /node command
int function_node() {
	char* str_pt_in;
	int i, j, cmd_max;
	X32command* command;
	// received a /node~~~,s~~[string] command
	// parse [string]
	// reply with node~~~~,s~~/[string] <data>...\n
	cmd_max = 0;
	str_pt_in = r_buf + 12;				// data block starts at index 12
	for (i = 0; i < Xnode_max; i++) {
		if (strncmp(Xnode[i].command, str_pt_in, Xnode[i].nchars) == 0) {
			cmd_max = Xnode[i].cmd_max;
			command = Xnode[i].cmd_ptr;
			break;
		}
	}
	if (i == Xnode_max)
		return 0;
	for (i = 0; i < cmd_max; i++) {
		if (command[i].flags == F_FND) {
//			printf("%s\n", command[i].command);
			if (strncmp(str_pt_in, command[i].command + 1, strlen(str_pt_in)) == 0) {
				s_len = Xsprint(s_buf, 0, 's', "node");
				s_len = Xsprint(s_buf, s_len, 's', ",s");
				s_buf[s_len] = 0;
				// manage head of nodes (two of more consecutive F_FND types)
				while (command[i + 1].flags == F_FND)
					i++;
				// we're now pointing at the first data command (with parameters) of the node pack
				strcat(s_buf + s_len, command[i].command);
				switch (command[i].format.typ) {
				case OFFON:
				case SSOLOSW:
					for (j = 1; j < command[i].value.ii + 1; j++) {
						strcat(s_buf + s_len, command[i + j].value.ii ? " ON" : " OFF");
					}
					break;
				case CMONO:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " LCR" : " LR+M");
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					break;
				case CSOLO:
					strcat(s_buf + s_len, Slevel(command[i + 1].value.ff));
					strcat(s_buf + s_len, Ssource[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 3].value.ff, -18., +18., 1));
					strcat(s_buf + s_len, command[i + 4].value.ii ? " AFL" : " PFL");
					strcat(s_buf + s_len, command[i + 5].value.ii ? " AFL" : " PFL");
					strcat(s_buf + s_len, command[i + 6].value.ii ? " AFL" : " PFL");
					strcat(s_buf + s_len, command[i + 7].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 8].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 9].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 10].value.ff, -40., 0., 0));
					strcat(s_buf + s_len, command[i + 11].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 12].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 13].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 14].value.ff, 0.3, 500., 1));
					strcat(s_buf + s_len, command[i + 15].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 16].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 17].value.ii ? " ON" : " OFF");
					break;
				case CTALK:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 2].value.ii ? " EXT" : " INT");
					break;
				case CTALKAB:
					strcat(s_buf + s_len, Slevel(command[i + 1].value.ff));
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sbitmp(command[i + 4].value.ii, 18));
					break;
				case COSC:
					strcat(s_buf + s_len, Slevel(command[i + 1].value.ff));
					strcat(s_buf + s_len, f121[(int) (120 * command[i + 2].value.ff + 0.5)]);
					strcat(s_buf + s_len, f121[(int) (120 * command[i + 3].value.ff + 0.5)]);
					strcat(s_buf + s_len, command[i + 4].value.ii ? " F2" : " F1");
					strcat(s_buf + s_len, Sosct[command[i + 5].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 6].value.ii));
					break;
				case CROUTSW:
					strcat(s_buf + s_len, Sroutin[command[i + 1].value.ii]);
					break;
				case CROUTIN:
				case CROUTPLAY:
					strcat(s_buf + s_len, Sroutin[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Sroutin[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sroutin[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sroutin[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Sroutax[command[i + 5].value.ii]);
					break;
				case CROUTAC:
					for (j = 1; j < command[i].value.ii + 1; j++) {
						strcat(s_buf + s_len, Sroutac[command[i + j].value.ii]);
					}
					break;
				case CROUTOT:
					strcat(s_buf + s_len, Srouto1[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Srouto1[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Srouto2[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Srouto2[command[i + 4].value.ii]);
					break;
				case CCTRL:
					strcat(s_buf + s_len, Scolor[command[i + 1].value.ii]);
					break;
				case CENC:
					for (j = 1; j < command[i].value.ii + 1; j++) {
						if (command[i + j].value.str) {
							strcat(s_buf + s_len, " \"");
							strcat(s_buf + s_len, command[i + j].value.str);
							strcat(s_buf + s_len, "\"");
						} else
							strcat(s_buf + s_len, " \"-\"");
					}
					break;
				case CTAPE:
					strcat(s_buf + s_len, Slinf(command[i + 1].value.ff, -6., +24., 1));
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, -6., +24., 1));
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					break;
				case CMIX:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					break;
				case CHCO:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Scolor[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					break;
				case CHDE:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, 0.3, 500., 1));
					break;
				case CHPR:
					strcat(s_buf + s_len, Slinfs(command[i + 1].value.ff, -18., +18., 1));
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sfslope[command[i + 4].value.ii]);
					strcat(s_buf + s_len, f101[(int) (100 * command[i + 5].value.ff + 0.5)]);
					break;
				case CHGA:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sgmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 3].value.ff, -80., 0., 1));
					strcat(s_buf + s_len, Slinf(command[i + 4].value.ff, 3., 60., 1));
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, 0., 120., 0));
					strcat(s_buf + s_len, Slogf(command[i + 6].value.ff, 0.02, 2000., 2));
					strcat(s_buf + s_len, Slogf(command[i + 7].value.ff, 5., 4000., 0));
					strcat(s_buf + s_len, Sint(command[i + 8].value.ii));
					break;
				case CHGF:
				case CHDF:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sgftype[command[i + 2].value.ii]);
					strcat(s_buf + s_len, f201[(int) (200 * command[i + 3].value.ff + 0.5)]);
					break;
				case CHDY:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sdmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sddet[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sdenv[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, -60., 0., 1));
					strcat(s_buf + s_len, Sdratio[command[i + 6].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 7].value.ff, 0., 5., 0));
					strcat(s_buf + s_len, Slinf(command[i + 8].value.ff, 0., 24., 1));
					strcat(s_buf + s_len, Slinf(command[i + 9].value.ff, 0., 120., 0));
					strcat(s_buf + s_len, Slogf(command[i + 10].value.ff, 0.02, 2000., 2));
					strcat(s_buf + s_len, Slogf(command[i + 11].value.ff, 5., 4000., 0));
					strcat(s_buf + s_len, Sdpos[command[i + 12].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 13].value.ii));
					strcat(s_buf + s_len, Slinf(command[i + 14].value.ff, 0., 100., 0));
					strcat(s_buf + s_len, command[i + 15].value.ii ? " ON" : " OFF");
					break;
				case CHIN:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sdpos[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sinsel[command[i + 3].value.ii]);
					break;
				case CHEQ:
					strcat(s_buf + s_len, Setype[command[i + 1].value.ii]);
					strcat(s_buf + s_len, f201[(int) (200 * command[i + 2].value.ff + 0.5)]);
					strcat(s_buf + s_len, Slinfs(command[i + 3].value.ff, -15., +15., 2));
					strcat(s_buf + s_len, Slogf(command[i + 4].value.ff, 10., 0.315, 1));
					break;
				case CHMX:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slinfs(command[i + 4].value.ff, -100., +100., 0));
					strcat(s_buf + s_len, command[i + 5].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 6].value.ff));
					break;
				case CHMO:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					strcat(s_buf + s_len, Slinfs(command[i + 3].value.ff, -100., +100., 0));
					strcat(s_buf + s_len, Sctype[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));
					break;
				case CHME:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					break;
				case CHGRP:
					strcat(s_buf + s_len, Sbitmp(command[i + 1].value.ii, 8));
					strcat(s_buf + s_len, Sbitmp(command[i + 2].value.ii, 6));
					break;
				case CHAMIX:
					strcat(s_buf + s_len, Samix[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Slinfs(command[i + 2].value.ff, -12., +12., 1));
					break;
				case AXPR:
					strcat(s_buf + s_len, Slinf(command[i + 1].value.ff, -18., +18., 1));
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					break;
				case BSCO:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Scolor[command[i + 3].value.ii]);
					break;
				case MXPR:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					break;
				case MXDY:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sdmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sddet[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sdenv[command[i + 4].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, -60., 0., 1));
					strcat(s_buf + s_len, Sdratio[command[i + 6].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 7].value.ff, 0., 5., 0));
					strcat(s_buf + s_len, Slinf(command[i + 8].value.ff, 0., 24., 1));
					strcat(s_buf + s_len, Slinf(command[i + 9].value.ff, 0., 120., 0));
					strcat(s_buf + s_len, Slogf(command[i + 10].value.ff, 0.02, 2000., 2));
					strcat(s_buf + s_len, Slogf(command[i + 11].value.ff, 5., 4000., 0));
					strcat(s_buf + s_len, Sdpos[command[i + 12].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 14].value.ff, 0., 100., 0));
					strcat(s_buf + s_len, command[i + 15].value.ii ? " ON" : " OFF");
					break;
				case MSMX:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slevel(command[i + 2].value.ff));
					strcat(s_buf + s_len, Slinfs(command[i + 4].value.ff, -100., +100., 0));
					break;
				case FXTYP1:
					strcat(s_buf + s_len, Sfxtyp1[command[i + 1].value.ii]);
					break;
				case FXSRC:
					strcat(s_buf + s_len, Sfxsrc[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Sfxsrc[command[i + 2].value.ii]);
					break;
				case FXPAR1:
					GetFxPar1(command, s_buf + s_len, i + 1, command[i - 4].value.ii);
					break;
				case FXTYP2:
					strcat(s_buf + s_len, Sfxtyp2[command[i + 1].value.ii]);
					break;
				case FXPAR2:
					GetFxPar1(command, s_buf + s_len, i + 1, command[i - 2].value.ii + _1_PIT + 2);
					break;
				case OMAIN:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					strcat(s_buf + s_len, Smpos[command[i + 2].value.ii]);
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					break;
				case OMAIN2:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					strcat(s_buf + s_len, Smpos[command[i + 2].value.ii]);
					break;
				case OP16:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					break;
				case OMAIND:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, 0.3, 500., 1));
					break;
				case HAMP:
					strcat(s_buf + s_len, Slinf(command[i + 1].value.ff, -12., 60., 1));
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					break;
				case PREFS:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, 10., 100., 0));
					strcat(s_buf + s_len, Slinf(command[i + 3].value.ff, 0., 100., 0));
					strcat(s_buf + s_len, Slinf(command[i + 4].value.ff, 10., 100., 0));
					strcat(s_buf + s_len, Slinf(command[i + 5].value.ff, 10., 100., 0));
					strcat(s_buf + s_len, command[i + 6].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 7].value.ii ? " 44k1" : " 48k");
					strcat(s_buf + s_len, Psource[command[i + 8].value.ii]);
					strcat(s_buf + s_len, command[i + 9].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 10].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 11].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 12].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 13].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 14].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 15].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sbitmp(command[i + 16].value.ii, 4));
					strcat(s_buf + s_len, command[i + 17].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, PSCont[command[i + 18].value.ii]);
					strcat(s_buf + s_len, command[i + 19].value.ii ? " 12h" : " 24h");
					strcat(s_buf + s_len, command[i + 20].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 21].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 22].value.ii ? " INV" : " NORM");
					if (command[i + 23].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 23].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					break;
				case PIR:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, PRpro[command[i + 2].value.ii]);
					strcat(s_buf + s_len, PRport[command[i + 3].value.ii]);
					strcat(s_buf + s_len, Sbitmp(command[i + 4].value.ii, 12));
					break;
				case PIQ:
					strcat(s_buf + s_len, XiQspk[command[i + 1].value.ii]);
					strcat(s_buf + s_len, XiQeq[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					break;
				case PCARD:
					strcat(s_buf + s_len, Pctype[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Pufmode[command[i + 2].value.ii]);
					strcat(s_buf + s_len, Pusbmod[command[i + 3].value.ii]);
					strcat(s_buf + s_len, command[i + 4].value.ii ? " OUT" : " IN");
					strcat(s_buf + s_len, Pcas[command[i + 5].value.ii]);
					strcat(s_buf + s_len, command[i + 6].value.ii ? " 64" : " 56");
					strcat(s_buf + s_len, Pcmadi[command[i + 7].value.ii]);
					strcat(s_buf + s_len, Pcmado[command[i + 8].value.ii]);
					strcat(s_buf + s_len, Pmadsrc[command[i + 9].value.ii]);
					break;
				case PRTA:
					strcat(s_buf + s_len, Prtavis[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Slinf(command[i + 2].value.ff, 0., 60., 0));
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					strcat(s_buf + s_len, command[i + 5].value.ii ? "POST" : " PRE");
					strcat(s_buf + s_len, command[i + 6].value.ii ? " SPEC" : " BAR");
					strcat(s_buf + s_len, Sbitmp(command[i + 7].value.ii, 6));
					strcat(s_buf + s_len, command[i + 8].value.ii ? " PEAK" : " RMS");
					strcat(s_buf + s_len, Slogf(command[i + 9].value.ff, 0.25, 16., 2));
					strcat(s_buf + s_len, Prtaph[command[i + 10].value.ii]);
					break;
				case PIP:
					strcat(s_buf + s_len, command[i + 1].value.ii ? " ON" : " OFF");
					break;
				case PKEY:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					for (j = 0; j < 1; j++) {
						if (command[i + 2 + j].value.str) {
							strcat(s_buf + s_len, " \"");
							strcat(s_buf + s_len, command[i + 2 + j].value.str);
							strcat(s_buf + s_len, "\"");
						} else {
							strcat(s_buf + s_len, " \" \"");
						}
					}
					break;
				case PADDR:
				case PMASK:
				case PGWAY:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					break;
				case STAT:
					strcat(s_buf + s_len, Sselidx[command[i + 1].value.ii]);
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, command[i + 4].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 6].value.ii));
					strcat(s_buf + s_len, command[i + 7].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 8].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sint(command[i + 9].value.ii));
					strcat(s_buf + s_len, command[i + 10].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 11].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 12].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 13].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 14].value.ii ? " SPEC" : " BAR");
					strcat(s_buf + s_len, command[i + 15].value.ii ? " SPEC" : " BAR");
					strcat(s_buf + s_len, command[i + 16].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 17].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sint(command[i + 18].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 19].value.ii));
					strcat(s_buf + s_len, command[i + 20].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 21].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, Sint(command[i + 22].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 23].value.ii));
					break;
				case SSCREEN:
					strcat(s_buf + s_len, Sscrn[command[i + 1].value.ii]);
					strcat(s_buf + s_len, command[i + 2].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len, command[i + 3].value.ii ? " ON" : " OFF");
					break;
				case SCHA:
					strcat(s_buf + s_len, Schal[command[i + 1].value.ii]);
					break;
				case SMET:
					strcat(s_buf + s_len, Smetl[command[i + 1].value.ii]);
					break;
				case SROU:
					strcat(s_buf + s_len, Sroul[command[i + 1].value.ii]);
					break;
				case SSET:
					strcat(s_buf + s_len, Ssetl[command[i + 1].value.ii]);
					break;
				case SLIB:
					strcat(s_buf + s_len, Slibl[command[i + 1].value.ii]);
					break;
				case SFX:
					strcat(s_buf + s_len, Sfxl[command[i + 1].value.ii]);
					break;
				case SMON:
					strcat(s_buf + s_len, Smonl[command[i + 1].value.ii]);
					break;
				case SUSB:
					strcat(s_buf + s_len, Susbl[command[i + 1].value.ii]);
					break;
				case SSCE:
					strcat(s_buf + s_len, Sscel[command[i + 1].value.ii]);
					break;
				case SASS:
					strcat(s_buf + s_len, Sassl[command[i + 1].value.ii]);
					break;
				case SAES:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \" \"");
					if (command[i + 2].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 2].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \" \"");
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					break;
				case STAPE:
					strcat(s_buf + s_len, Stapl[command[i + 1].value.ii]);
					if (command[i + 2].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 2].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					break;
				case SOSC:
					strcat(s_buf + s_len,
							command[i + 1].value.ii ? " ON" : " OFF");
					break;
				case STALK:
					strcat(s_buf + s_len,
							command[i + 1].value.ii ? " ON" : " OFF");
					strcat(s_buf + s_len,
							command[i + 2].value.ii ? " ON" : " OFF");
					break;
				case USB:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					if (command[i + 2].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 2].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					break;
				case SNAM:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 6].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 7].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 8].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 9].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 10].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 11].value.ii));
					strcat(s_buf + s_len, " \"");
					strcat(s_buf + s_len, XVERSION);
					strcat(s_buf + s_len, "\"");
					break;
				case SCUE:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					if (command[i + 2].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 2].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 6].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 7].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 8].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 9].value.ii));
					break;
				case SSCN:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					if (command[i + 2].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 2].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sbitmp(command[i + 3].value.ii, 9));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					break;
				case SSNP:
					if (command[i + 1].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 1].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 6].value.ii));
					break;
				case HA:
					break;
				case ACTION:
					break;
				case UREC:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 4].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));

					strcat(s_buf + s_len, Ubat[command[i + 6].value.ii]);

					strcat(s_buf + s_len, Sint(command[i + 7].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 8].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 9].value.ii));
					strcat(s_buf + s_len, Sint(command[i + 10].value.ii));

					strcat(s_buf + s_len, Usdc[command[i + 11].value.ii]);
					strcat(s_buf + s_len, Usdc[command[i + 12].value.ii]);

					if (command[i + 13].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 13].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");

					if (command[i + 14].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 14].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");

					if (command[i + 15].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 15].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");

					strcat(s_buf + s_len, Sint(command[i + 16].value.ii));
					break;
				case SLIBS:
					strcat(s_buf + s_len, Sint(command[i + 1].value.ii));
					if (command[i + 2].value.str) {
						strcat(s_buf + s_len, " \"");
						strcat(s_buf + s_len, command[i + 2].value.str);
						strcat(s_buf + s_len, "\"");
					} else
						strcat(s_buf + s_len, " \"\"");
					strcat(s_buf + s_len, Sint(command[i + 3].value.ii));
					strcat(s_buf + s_len, Sbitmp(command[i + 4].value.ii, 16));
					strcat(s_buf + s_len, Sint(command[i + 5].value.ii));
					break;
				case D48:
					strcat(s_buf + s_len, Sbitmp(command[i + 1].value.ii, 4));
					strcat(s_buf + s_len, Sint(command[i + 2].value.ii));
					break;
				case D48A:
					for (j = 1; j < 49; j++) {
						strcat(s_buf + s_len, Sint(command[i + j].value.ii));
					}
					break;
				case D48G:
					for (j = 1; j < 13; j++) {
						if (command[i + j].value.str) {
							strcat(s_buf + s_len, " \"");
							strcat(s_buf + s_len, command[i + j].value.str);
							strcat(s_buf + s_len, "\"");
						} else
							strcat(s_buf + s_len, " \"\"");					}
					break;
				case UROUO:
					for (j = 1; j < 49; j++) {
						strcat(s_buf + s_len, Sint(command[i + j].value.ii));
					}
					break;
				case UROUI:
					for (j = 1; j < 33; j++) {
						strcat(s_buf + s_len, Sint(command[i + j].value.ii));
					}
					break;
				default:
					return 0;
					break;
				}
				s_len += strlen(s_buf + s_len);
				s_buf[s_len++] = '\n';
				s_buf[s_len++] = 0;
				while (s_len & 3) s_buf[s_len++] = 0;
				return S_SND; // send reply only to requesting client
			}
		} else {
			// Trying to re-use what's already there.
			// We have data coming in - Parse!
			if (strncmp(str_pt_in, command[i].command + 1, strlen(str_pt_in)) == 0) {
				i = s_len = p_status = 0;
				// change the command as if it were sent as a single command...
				// for example on the command
				//     /node~~~,s~~-prefs/rta/visibility
				// we change to to
				//     /-prefs/rta/visibility
				// and send the command for parsing...
				if (*(r_buf + 12) == '/') {
					memcpy(r_buf + 1, r_buf + 13, r_len - 13);
				} else {
					memcpy(r_buf + 1, r_buf + 12, r_len - 12);
				}
				r_len -= 12;
				// Parse the command; this will update the Send buffer (and send buffer number of bytes)
				// and the parsing status in p_status
				while (i < Xheader_max) {
					if (Xheader[i].header.icom == (int) *((int*) v_buf)) { // single int test!
						p_status = Xheader[i].fptr(); // call associated parsing function
						break; // Done parsing, exit parsing while loop
					}
					i += 1;
				}
				if (i < Xheader_max) return function_node_single(i);
			}
		}
	}
	return 0; // no reply if error detected
}
//
// Single node function - reply to /node (single argument) reply with appropriate data
int function_node_single() {

X32command	*command = node_single_command;
int			index = node_single_index;
	// Global variable node_single_index represents the function index
	// s_buf & s_len contain a reply that won't work as the expected output is not in
	// the form of an OSC command reply,
	//
	// change the buffer to send... depending on the parameter type
	// save s_buf in r_buf (not needed anymore)
	memcpy(r_buf, s_buf, s_len);
	r_len = s_len;
	s_len = 12;
	memcpy(s_buf, "node\0\0\0\0,s\0\0", s_len);	// Set command reply header
	strcpy(s_buf + s_len, r_buf); 				// append node including leading '/'
	s_len = strlen(r_buf) + s_len;
//			strcpy(s_buf + s_len, " 70%\n");			// test only!
//			s_len += 5;									// test only!

	if (command[index].node) {
		// array of enum strings available
		strcpy(s_buf + s_len, command[index].node[command[index].value.ii]);
		s_len += strlen(command[index].node[command[index].value.ii]);
	} else {
		// evaluate string from value/type
		if (command[index].format.typ == I32) {
			s_len += sprintf(s_buf + s_len, " %d", command[index].value.ii);
		} else if (command[index].format.typ == F32) {
			s_len += sprintf(s_buf + s_len, " %f", command[index].value.ff);
		} else if (command[index].format.typ == S32) {
			if (command[index].value.str) {
				strcpy(s_buf + s_len, command[index].value.str);
				s_len += strlen(command[index].value.str);
			}
		} else if (command[index].format.typ == P32) {
			s_len += sprintf(s_buf + s_len, " %%");
			int il = 31;
			while (il > 0) {
				if (command[index].value.ii & (1 << il)) break;
				il--;
			}
			for (; il >= 0; il--) {
				if (command[index].value.ii & (1 << il)) s_len += sprintf(s_buf + s_len, "1");
				else                                     s_len += sprintf(s_buf + s_len, "0");
			}
		} else {
			strcpy(s_buf + s_len, " TODO");
			s_len += 5;
		}
	}
	s_buf[s_len++] = '\n';
	s_buf[s_len++] = '\0';
	// pad output with '\0' to multiple of 4
	while (s_len & 3) s_buf[s_len++] = 0;
	// we now have to set a single parameter in the form of a string, ending with a line feed
	return S_SND; // send reply only to requesting client
}

//
// /config command
int function_config() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xconfig_max) {
		if (strcmp(r_buf, Xconfig[i].command) == 0) {
			// found command at index i
			return (funct_params(Xconfig, i));
		}
		i += 1;
	}
	return 0;
}

//
// /main command
int function_main() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xmain_max) {
		if (strcmp(r_buf, Xmain[i].command) == 0) {
			// found command at index i
			return (funct_params(Xmain, i));
		}
		i += 1;
	}
	return 0;
}

//
// /-prefs command
int function_prefs() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xprefs_max) {
		if (strcmp(r_buf, Xprefs[i].command) == 0) {
			// found command at index i
			return (funct_params(Xprefs, i));
		}
		i += 1;
	}
	return 0;
}

//
// /-stat command
int function_stat() {
	int i;
//
// check for lock command
	if (strstr(r_buf, "/lock")) {
		// shutdown is /-stat/lock ,i 2, that's int 0x0002 at index 19
		if (r_buf[19] == 2) return X32Shutdown();
	}
// check for other -stat commands
	i = 0;
	while (i < Xstat_max) {
		if (strcmp(r_buf, Xstat[i].command) == 0) {
			// found command at index i
			return (funct_params(Xstat, i));
		}
		i += 1;
	}
	return 0;
}

//
// /-urec command
int function_urec() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xurec_max) {
		if (strcmp(r_buf, Xurec[i].command) == 0) {
			// found command at index i
			return (funct_params(Xurec, i));
		}
		i += 1;
	}
	return 0;
}

//
// /ch command
int function_channel() {
	int i;
	X32command *Xchannel;
//
// check for actual command
// manage 32 channels 01 to 32 - /ch/xx/yyy
	i = (r_buf[4] - 48) * 10 + r_buf[5] - 48 - 1;
	if ((i < 0) || (i > 31)) return 0;
	Xchannel = Xchannelset[i];
	i = 0;
	while (i < Xchannel01_max) {
		if (strcmp(r_buf, Xchannel[i].command) == 0) {
			// found command at index i
			return (funct_params(Xchannel, i));
		}
		i += 1;
	}
	return 0;
}

//
// /auxin command
int function_auxin() {
	int i;
	X32command *Xauxin;
//
// check for actual command
// manage 8 auxin 01 to 08 - /auxin/xx/yyy
	i = (r_buf[7] - 48) * 10 + r_buf[8] - 48 - 1;
	if ((i < 0) || (i > 7)) return 0;
	Xauxin = Xauxinset[i];
	i = 0;
	while (i < Xauxin01_max) {
		if (strcmp(r_buf, Xauxin[i].command) == 0) {
			// found command at index i
			return (funct_params(Xauxin, i));
		}
		i += 1;
	}
	return 0;
}

//
// /fxrtn command
int function_fxrtn() {
	int i;
	X32command *Xfxrtn;
//
// check for actual command
// manage 8 fxrtn 01 to 08 - /fxrtn/xx/yyy
	i = (r_buf[7] - 48) * 10 + r_buf[8] - 48 - 1;
	if ((i < 0) || (i > 7)) return 0;
	Xfxrtn = Xfxrtnset[i];
	i = 0;
	while (i < Xfxrtn01_max) {
		if (strcmp(r_buf, Xfxrtn[i].command) == 0) {
			// found command at index i
			return (funct_params(Xfxrtn, i));
		}
		i += 1;
	}
	return 0;
}

//
// /bus command
int function_bus() {
	int i;
	X32command *Xbus;
//
// check for actual command
// manage 16 bus 01 to 16 - /bus/xx/yyy
	i = (r_buf[5] - 48) * 10 + r_buf[6] - 48 - 1;
	if ((i < 0) || (i > 15)) return 0;
	Xbus = Xbusset[i];
	i = 0;
	while (i < Xbus01_max) {
		if (strcmp(r_buf, Xbus[i].command) == 0) {
			// found command at index i
			return (funct_params(Xbus, i));
		}
		i += 1;
	}
	return 0;
}

//
// /mtx command
int function_mtx() {
	int i;
	X32command *Xmtx;
//
// check for actual command
// manage 6 mtx 01 to 06 - /mtx/xx/yyy
	i = (r_buf[5] - 48) * 10 + r_buf[6] - 48 - 1;
	if ((i < 0) || (i > 5)) return 0;
	Xmtx = Xmtxset[i];
	i = 0;
	while (i < Xmtx01_max) {
		if (strcmp(r_buf, Xmtx[i].command) == 0) {
			// found command at index i
			return (funct_params(Xmtx, i));
		}
		i += 1;
	}
	return 0;
}

//
// /dca command
int function_dca() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xdca_max) {
		if (strcmp(r_buf, Xdca[i].command) == 0) {
			// found command at index i
			return (funct_params(Xdca, i));
		}
		i += 1;
	}
	return 0;
}

//
// /fx command
int function_fx() {
	int i, fx;
	X32command *Xfx;
//
// check for actual command
// manage 8 fx 1 to 8 - /fx/x/yyy
	fx = r_buf[4] - 48 - 1;
	if ((fx < 0) || (fx > 7)) return 0;
	Xfx = Xfxset[fx];
	i = 0;
	if (fx < 4) {
		while (i < Xfx1_max) {
			if (strcmp(r_buf, Xfx[i].command) == 0) {
				// found command at index i
				return (funct_params(Xfx, i));
			}
			i += 1;
		}
	} else {
		while (i < Xfx5_max) {
			if (strcmp(r_buf, Xfx[i].command) == 0) {
				// found command at index i
				return (funct_params(Xfx, i));
			}
			i += 1;
		}
	}
	return 0;
}

//
// /output command
int function_output() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xoutput_max) {
		if (strcmp(r_buf, Xoutput[i].command) == 0) {
			// found command at index i
			return (funct_params(Xoutput, i));
		}
		i += 1;
	}
	return 0;
}

//
// /headamp command
int function_headamp() {
	int i;
	X32command *Xheadmp;

//
	// check for actual command
	// manage 8 headamp 000 to 127 - /headamp/yyy/...
	i = (r_buf[9] - 48) * 100 + (r_buf[10] - 48) * 10 + r_buf[11] - 48;
	if ((i < 0) || (i > 127)) return 0;
	Xheadmp = Xheadmpset[i];
	i = 0;
	while (i < Xheadamp_max) {
		if (strcmp(r_buf, Xheadmp[i].command) == 0) {
			// found command at index i
			return (funct_params(Xheadmp, i));
		}
		i += 1;
	}
	return 0;
}

//
//
void Xprepmeter(int i, int l, char *buf, int n, int k) {
	// prepare (fake) meters/i command reply
	ZMemory(&Xbuf_meters[i][0], 512);			// Prepare buffer (set to all 0's)
	memcpy(&Xbuf_meters[i][0], buf, 16);

	endian.ii = (l + 1) * 4;		// actual blob content length (in bytes)
	Xbuf_meters[i][16] = endian.cc[3];
	Xbuf_meters[i][17] = endian.cc[2];
	Xbuf_meters[i][18] = endian.cc[1];
	Xbuf_meters[i][19] = endian.cc[0];

	Lbuf_meters[i] = endian.ii + 20;		// length of the whole message (in bytes)

	endian.ii = l; // number of floats (32-bit)
	Xbuf_meters[i][20] = endian.cc[0];
	Xbuf_meters[i][21] = endian.cc[1];
	Xbuf_meters[i][22] = endian.cc[2];
	Xbuf_meters[i][23] = endian.cc[3];

	gettimeofday (&XTimerMeters[i], NULL);		// get time
	XInterMeters[i] = XTimerMeters[i];			// keep initial time for inter-timers
	XTimerMeters[i].tv_sec += 10;				// keep valid for 10s
	XActiveMeters |= (1 << i);					// set meter to active
	XClientMeters[i] = *Client_ip_pt;			// remember requesting IP client TODO: not the right approach
												//									   if several clients request meters
	if (n == 1) {								// special case of a single shot for meters/i
		XDeltaMeters[i] = 50000;				// set meter interval at 50ms
		return; 								// meters/i set only to requesting client
	} else {
		// get time factor at end of command /meters ,si /meters/i [tf]
		// manage values < 1 and > 99 by setting interval to 50ms
		// k represents an index afetr 24 where to find the time factor data
		endian.cc[3] = r_buf[k + 24];
		endian.cc[2] = r_buf[k + 25];
		endian.cc[1] = r_buf[k + 26];
		endian.cc[0] = r_buf[k + 27];
		if ((endian.ii < 1) || (endian.ii > 99)) endian.ii = 1;
		XDeltaMeters[i] = 50000 * endian.ii;	// set meter interval at 50ms x time factor
		return; 								// meters/i set only to requesting client
	}
	return;
}

//
// meters command (/meters, ...)
int function_meters() {
	int i, n;
//
// parse /meters for actual command starts at index 12 or 16 in r_buf, depending on # of parameters
	n = strlen(r_buf + 9);			// counting the number of parameters
	i = 0;
	while (i < Xmeters_max) {
		if (strcmp(r_buf + ((9 + n + 4) & ~3), Xmeters[i].command) == 0) {
			// found command at index i
			break;
		}
		i += 1;
	}
	if (i < Xmeters_max) switch(i) {
		case 0:
			Xprepmeter(0, 70, "/meters/0\0\0\0,b\0\0", n, 0);
			break;
		case 1:
			Xprepmeter(1, 96, "/meters/1\0\0\0,b\0\0", n, 0);
			break;
		case 2:
			Xprepmeter(2, 49, "/meters/2\0\0\0,b\0\0", n, 0);
			break;
		case 3:
			Xprepmeter(3, 22, "/meters/3\0\0\0,b\0\0", n, 0);
			break;
		case 4:
			Xprepmeter(4, 82, "/meters/4\0\0\0,b\0\0", n, 0);
			break;
		case 5:
			Xprepmeter(5, 27, "/meters/5\0\0\0,b\0\0", n, 12);
			break;
		case 6:
			Xprepmeter(6, 4, "/meters/6\0\0\0,b\0\0", n, 8);
			break;
		case 7:
			Xprepmeter(7, 16, "/meters/7\0\0\0,b\0\0", n, 0);
			break;
		case 8:
			Xprepmeter(8, 6, "/meters/8\0\0\0,b\0\0", n, 0);
			break;
		case 9:
			Xprepmeter(9, 32, "/meters/9\0\0\0,b\0\0", n, 0);
			break;
		case 10:
			Xprepmeter(10, 32, "/meters/10\0\0,b\0\0", n, 0);
			break;
		case 11:
			Xprepmeter(11, 5, "/meters/11\0\0,b\0\0", n, 0);
			break;
		case 12:
			Xprepmeter(12, 4, "/meters/12\0\0,b\0\0", n, 0);
			break;
		case 13:
			Xprepmeter(13, 48, "/meters/13\0\0,b\0\0", n, 0);
			break;
		case 14:
			Xprepmeter(14, 80, "/meters/14\0\0,b\0\0", n, 0);
			break;
		case 15:
			Xprepmeter(15, 50, "/meters/15\0\0,b\0\0", n, 0);
			break;
		case 16:
			Xprepmeter(16, 48, "/meters/16\0\0,b\0\0", n, 0);
			break;
		default:
			break;
	}
	return 0;
}

//
// Other functions (/-usb, ...) and commands
int function_misc() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xmisc_max) {
		if (strcmp(r_buf, Xmisc[i].command) == 0) {
			// found command at index i
			return (funct_params(Xmisc, i));
		}
		i += 1;
	}
	return 0;
}

//
// Other functions (/renew) and commands
int function_renew() {
//
// Ignored for now / Todo
	return S_SND;
}
//
// /-action command
int function_action() {
	int i;
//
// check for actual command
	i = 0;
	while (i < Xaction_max) {
		if (strcmp(r_buf, Xaction[i].command) == 0) {
			// found command at index i
			return (funct_params(Xaction, i));
		}
		i += 1;
	}
	return 0;
}

//
// /-show command
int function_show() {
	int i;
//
// check for actual command
	i = 0;
	if (strncmp(r_buf + 16, "scene", 5) == 0) {
		while (i < Xscene_max) {
			if (strcmp(r_buf, Xscene[i].command) == 0) {
				// found command at index i
				return (funct_params(Xscene, i));
			}
			i += 1;
		}
		return 0;
	}
	if (strncmp(r_buf + 16, "snippet", 7) == 0) {
		while (i < Xsnippet_max) {
			if (strcmp(r_buf, Xsnippet[i].command) == 0) {
				// found command at index i
				return (funct_params(Xsnippet, i));
			}
			i += 1;
		}
		return 0;
	}
	while (i < Xshow_max) {
		if (strcmp(r_buf, Xshow[i].command) == 0) {
			// found command at index i
			return (funct_params(Xshow, i));
		}
		i += 1;
	}
	return 0;
}

//
// /copy command
int function_copy() {
	int i, j, srce, dest, mask;
	X32command *ch_src, *ch_dst;
//
// Copy function - format is /copy~~~,siii~~~type source destination mask
//
// At present time, /copy only implements 'libchan'
// TODO: 'scene', 'libfx', 'librout' should be implemented
// 'snippet' is not supported on X32
	i = 0;
	if (strcmp(r_buf + 16, "libchan") == 0) {
		i = 24;
		j = 4;
		while (j)
			endian.cc[--j] = r_buf[i++];
		srce = endian.ii;
		j = 4;
		while (j)
			endian.cc[--j] = r_buf[i++];
		dest = endian.ii;
		j = 4;
		while (j)
			endian.cc[--j] = r_buf[i++];
		mask = endian.ii;
		//
		if ((srce >= 0) && (srce < 32) && (dest >= 0) && (dest < 32)) {
			// copy channel <srce> to channel <dest>, for bits in <mask>
			ch_src = Xchannelset[srce];
			ch_dst = Xchannelset[dest];

			if (mask & C_CONFIG) {
				//config: i= 1-7
				for (i = 0; i < 8; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_HA) {
				//preamp: i= 8-19
				for (i = 8; i < 20; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_GATE) {
				//gate: i= 20-32
				for (i = 20; i < 33; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_DYN) {
				//dyn: i= 33-52
				for (i = 33; i < 53; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_EQ) {
				//eq: i= 53-74
				for (i = 53; i < 75; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			if (mask & C_SEND) {
				//sends: i= 75-145
				for (i = 75; i < 146; i++) {
					ch_dst[i].value.ii = ch_src[i].value.ii;
				}
			}
			// All done
			s_len = Xsprint(s_buf, 0, 's', "/copy");
			s_len = Xsprint(s_buf, s_len, 's', ",si");
			s_len = Xsprint(s_buf, s_len, 's', "libchan");
			s_len = Xsprint(s_buf, s_len, 'i', &one);
			return S_SND;
		} else {
			// Error somewhere
			s_len = Xsprint(s_buf, 0, 's', "/copy");
			s_len = Xsprint(s_buf, s_len, 's', ",si");
			s_len = Xsprint(s_buf, s_len, 's', "libchan");
			s_len = Xsprint(s_buf, s_len, 'i', &zero);
			return S_SND;
		}
	}
	return 0;
}

//
// /add command
int function_add() {
//
// add function - format is /add~~~~,sis~~~~type number name
//
// TODO: do a proper implementation of the function
	printf("Nothing actually added\n");
	fflush(stdout);
	if (strcmp(r_buf + 16, "cue") == 0) {
		// test... do nothing, just return OK status
		s_len = Xsprint(s_buf, 0, 's', "/add");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "cue");
		s_len = Xsprint(s_buf, s_len, 'i', &one);
	}
	return S_SND;
}

//
// /load command
int function_load() {
//
// load function - format is /load~~~~,siii~~~~type number name
//
// TODO: do a proper implementation of the function
	printf("Nothing actually loaded\n");
	fflush(stdout);
	s_len = Xsprint(s_buf, 0, 's', "/load");
	s_len = Xsprint(s_buf, s_len, 's', ",si");
	s_len = Xsprint(s_buf, s_len, 's', "libchan");
	s_len = Xsprint(s_buf, s_len, 'i', &one);
	return S_SND;
}

//
// /save command
int function_save() {
	int i, j;
//
// save function - format is /save~~~,si[s|i,..]~~~type number name note value
// Only 'sce', 'snippet' are implemented. 'libchan' is OK too
// TODO: implement 'libfx' and 'librout'
	i = 0;
	if (strcmp(r_buf + 16, "scene") == 0) {
		i = 24;
		//get sscene index
		// /save~~~,siss~~~scene~~~[i] name note
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf(tmp_str, "/-show/showfile/scene/%03d/name", endian.ii);
		for (j = 0; j < Xscene_max; j++) {
			if ((strcmp(tmp_str, Xscene[j].command)) == 0) {
				// save name
				if (Xscene[j].value.str) free(Xscene[j].value.str);
				if ((s_len = strlen(r_buf + i)) > 0) {
					Xscene[j].value.str = malloc((s_len + 8) * sizeof(char));
					strncpy(Xscene[j].value.str, r_buf + i, s_len);
					Xscene[j].value.str[s_len] = 0;
				} else {
					Xscene[j].value.str = NULL;
				}
				// save note in j+1, note data is at i + s_len % 4
				i = (i + s_len + 4) & 0xfffffffc;
				if (Xscene[j + 1].value.str) free(Xscene[j + 1].value.str);
				if ((s_len = strlen(r_buf + i)) > 0) {
					Xscene[j + 1].value.str = malloc((s_len + 8) * sizeof(char));
					strncpy(Xscene[j + 1].value.str, r_buf + i, s_len);
					Xscene[j + 1].value.str[s_len] = 0;
				} else {
					Xscene[j + 1].value.str = NULL;
				}
				//
				// update remote clients
				s_len = Xfprint(s_buf, 0, Xscene[j].command, 's', Xscene[j].value.str);
				Xsend(S_REM); // update xremote clients
				s_len = Xfprint(s_buf, 0, Xscene[j + 1].command, 's', Xscene[j + 1].value.str);
				Xsend(S_REM); // update xremote clients
				//
				// collect and save (locally) scene data
				// save corresponding data as nodes in "hasdata" indicator (at j+2)
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				Xscene[j + 2].value.ii = 1;
				s_len = Xfprint(s_buf, 0, Xscene[j + 2].command, 'i', &one);
				Xsend(S_REM); // update xremote clients
				//
				s_len = Xsprint(s_buf, 0, 's', "/save");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "scene");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				return S_SND;
			}
		}
		//Scene not found
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	} else if (strcmp(r_buf + 16, "snippet") == 0) {
		i = 24;
		//get snippet index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf(tmp_str, "/-show/showfile/snippet/%03d/name", endian.ii);
		for (j = 0; j < Xsnippet_max; j++) {
			if ((strcmp(tmp_str, Xsnippet[j].command)) == 0) {
				if (Xsnippet[j].value.str) free(Xsnippet[j].value.str);
				if ((s_len = strlen(r_buf + i)) > 0) {
					Xsnippet[j].value.str = malloc((s_len + 8) * sizeof(char));
					strncpy(Xsnippet[j].value.str, r_buf + i, s_len);
					Xsnippet[j].value.str[s_len] = 0;
				} else {
					Xsnippet[j].value.str = NULL;
				}
				//
				// TODO At this time the last two <string><int> parameters are ignored
				//
				// update remote clients
				s_len = Xfprint(s_buf, 0, tmp_str, 's', Xsnippet[j].value.str);
				Xsend(S_REM); // update xremote clients
				//
				// collect and save (locally) snippet data
				// save corresponding data as nodes in "hasdata" indicator (at j+1)
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				Xsnippet[j + 1].value.ii = 1;
				s_len = Xfprint(s_buf, 0, Xsnippet[j + 1].command, 'i', &one);
				Xsend(S_REM); // update xremote clients
				//
				s_len = Xsprint(s_buf, 0, 's', "/save");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "snippet");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				// update requesting client
				return S_SND;
			}
		}
		// Snippet not found
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "snippet");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	} else if (strcmp(r_buf + 16, "libchan") == 0) {
		printf("Nothing actually saved\n");
		fflush(stdout);
		i = 20;
		// test... do nothing, just return OK status
		s_len = Xsprint(s_buf, 0, 's', "/save");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "libchan");
		s_len = Xsprint(s_buf, s_len, 'i', &one);
		return S_SND;
	}
	return 0;
}

//
// /delete command
int function_delete() {
	int i, j;
//
// delete function - format is /delete~,si~~~~type number
	i = 0;
	if (strcmp(r_buf + 12, "scene") == 0) {
		i = 20;
		//get scene index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf(tmp_str, "/-show/showfile/scene/%03d/name", endian.ii);
		for (j = 0; j < Xscene_max; j++) {
			if ((strcmp(tmp_str, Xscene[j].command)) == 0) {
				// found name
				if (Xscene[j].value.str) free(Xscene[j].value.str);
				Xscene[j].value.str = NULL;
				if (Xscene[j + 1].value.str) free(Xscene[j + 1].value.str);
				Xscene[j + 1].value.str = NULL;
				Xscene[j + 2].value.ii = 0; // No data
				// update remote clients
				s_len = Xfprint(s_buf, 0, Xscene[j].command, 's', Xscene[j].value.str);
				Xsend(S_REM); // update xremote clients
				s_len = Xfprint(s_buf, 0, Xscene[j + 1].command, 's', Xscene[j + 1].value.str);
				Xsend(S_REM); // update xremote clients
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				s_len = Xfprint(s_buf, 0, Xscene[j + 2].command, 'i', &zero);
				Xsend(S_REM); // update xremote clients
				// return OK status
				s_len = Xsprint(s_buf, 0, 's', "/delete");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "scene");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				return S_SND;
			}
		}
		// Not found.. just return NOK status
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "scene");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	}
	if (strcmp(r_buf + 12, "snippet") == 0) {
		i = 20;
		//get snippet index
		j = 4;
		while (j) endian.cc[--j] = r_buf[i++];
		sprintf(tmp_str, "/-show/showfile/snippet/%03d/name", endian.ii);
		for (j = 0; j < Xsnippet_max; j++) {
			if ((strcmp(tmp_str, Xsnippet[j].command)) == 0) {
				// found name
				if (Xsnippet[j].value.str) free(Xsnippet[j].value.str);
				Xsnippet[j].value.str = NULL;
				Xsnippet[j + 1].value.ii = 0; // No data
				// update remote clients
				s_len = Xfprint(s_buf, 0, Xsnippet[j].command, 's', Xsnippet[j].value.str);
				Xsend(S_REM); // update xremote clients
				// todo: this is not actually done here!
				// hasdata should point to .data and not to .ii and a set of node commands should be saved
				s_len = Xfprint(s_buf, 0, Xsnippet[j + 1].command, 'i', &zero);
				Xsend(S_REM); // update xremote clients
				// return OK status
				s_len = Xsprint(s_buf, 0, 's', "/delete");
				s_len = Xsprint(s_buf, s_len, 's', ",si");
				s_len = Xsprint(s_buf, s_len, 's', "snippet");
				s_len = Xsprint(s_buf, s_len, 'i', &one);
				return S_SND;
			}
		}
		// Not found, return NOK status
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "snippet");
		s_len = Xsprint(s_buf, s_len, 'i', &zero);
		return S_SND;
	}
	if (strcmp(r_buf + 16, "libchan") == 0) {
		i = 20;
		// test... do nothing, just return OK status
		s_len = Xsprint(s_buf, 0, 's', "/delete");
		s_len = Xsprint(s_buf, s_len, 's', ",si");
		s_len = Xsprint(s_buf, s_len, 's', "libchan");
		s_len = Xsprint(s_buf, s_len, 'i', &one);
		return S_SND;
	}
	return 0;
}
//
// function_libs(): to manage /-libs commands
int function_libs() {
	printf("Doing nothing for /-libs\n");
	fflush(stdout);
	return 0;
}
//
// function_showdump(): mamging /showdump requests
int function_showdump() {
char shname[32];

	s_len = Xsprint(s_buf, 0, 's', "node");
	s_len = Xsprint(s_buf, s_len, 's', ",s");
	shname[0] = 0;
	if (Xshow[4].value.str) strcpy(shname, Xshow[4].value.str);		//Xshow[4] holds the current show name
	sprintf(r_buf, "/-show/showfile/show \"%s\" 0 0 0 0 0 0 0 0 0 0 \"%s\"", shname, XVERSION);
	s_len = Xsprint(s_buf, s_len, 's', r_buf);
	return S_SND;
}
//
// Shutdown: a function (non Behringer standard) to save all current emulator values and
// settings. Enables keeping data from one session to the next
int function_shutdown() {
	char* info = "osc-server\000\000";

	if (Xverbose) {
		Xfdump("Shutting down", info, 12, Xdebug);
		fflush(stdout);
	}
	return X32Shutdown();
}
//
//
//
#define save(xx)																		\
	fprintf(X32File, "%d ", xx[i].format.typ);											\
	if (xx[i].format.typ == S32) {														\
		if (xx[i].value.str != NULL) {													\
			fprintf(X32File, "%d %s\n", (int)strlen(xx[i].value.str), xx[i].value.str);	\
		} else {																		\
			fprintf(X32File, "%d\n", 0);												\
		}																				\
	} else {																			\
		fprintf(X32File, "%d\n", xx[i].value.ii);										\
	}

int X32Shutdown() {
	int i, j;
	X32command* Xarray;
	FILE *X32File;
//
	if ((X32File = fopen(".X32res.rc", "w")) == NULL)
		return 0;
	printf("saving init file...");
	fflush(stdout);
// read file init values for all data
	for (i = 0; i < Xconfig_max; i++) {
		save(Xconfig)
	}
	for (i = 0; i < Xmain_max; i++) {
		save(Xmain);
	}
	for (i = 0; i < Xprefs_max; i++) {
		save(Xprefs);
	}
	for (i = 0; i < Xstat_max; i++) {
		save(Xstat);
	}
	Xarray = Xchannelset[0];
	for (i = 0; i < Xchannel01_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 32; j++) {
		Xarray = Xchannelset[j];
		for (i = 0; i < Xchannel02_max; i++) {
			save(Xarray);
		}
	}
	Xarray = Xauxinset[0];
	for (i = 0; i < Xauxin01_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 8; j++) {
		Xarray = Xauxinset[j];
		for (i = 0; i < Xauxin02_max; i++) {
			save(Xarray);
		}
	}
	Xarray = Xfxrtnset[0];
	for (i = 0; i < Xfxrtn01_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 8; j++) {
		Xarray = Xfxrtnset[j];
		for (i = 0; i < Xfxrtn02_max; i++) {
			save(Xarray);
		}
	}
	Xarray = Xbusset[0];
	for (i = 0; i < Xbus01_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 16; j++) {
		Xarray = Xbusset[j];
		for (i = 0; i < Xbus02_max; i++) {
			save(Xarray);
		}
	}
	Xarray = Xmtxset[0];
	for (i = 0; i < Xmtx01_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 6; j++) {
		Xarray = Xmtxset[j];
		for (i = 0; i < Xmtx02_max; i++) {
			save(Xarray);
		}
	}
	for (i = 0; i < Xdca_max; i++) {
		save(Xdca);
	}
	Xarray = Xfxset[0];
	for (i = 0; i < Xfx1_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 8; j++) {
		Xarray = Xfxset[j];
		if (j < 4) {
			for (i = 0; i < Xfx2_max; i++) {
				save(Xarray);
			}
		} else {
			for (i = 0; i < Xfx5_max; i++) {
				save(Xarray);
			}
		}
	}
	for (i = 0; i < Xoutput_max; i++) {
		save(Xoutput);
	}
	Xarray = Xheadmpset[0];
	for (i = 0; i < Xheadamp_max; i++) {
		save(Xarray);
	}
	for (j = 1; j < 128; j++) {
		Xarray = Xheadmpset[j];
		for (i = 0; i < Xheadamp1_max; i++) {
			save(Xarray);
		}
	}
	for (i = 0; i < Xmisc_max; i++) {
		save(Xmisc);
	}
	for (i = 0; i < Xurec_max; i++) {
		save(Xurec);
	}
	for (i = 0; i < Xlibsc_max; i++) {
		save(Xlibsc);
	}
	for (i = 0; i < Xlibsr_max; i++) {
		save(Xlibsr);
	}
	for (i = 0; i < Xlibsf_max; i++) {
		save(Xlibsf);
	}
	fclose(X32File);
	printf(" Done\n");
	fflush(stdout);
	keep_on = 0;
	return 0;
}

#define restore(xx)																	\
	f_stat = fscanf(X32File, "%d ", &type);											\
	if (type == S32) {																\
		f_stat = fscanf(X32File, "%d ", &r_len);									\
		if (r_len > 0) {															\
			for (k = 0; k < r_len; k++) f_stat = fscanf(X32File, "%c", r_buf + k);	\
			if (xx[i].value.str) free(xx[i].value.str);								\
			xx[i].value.str = malloc(r_len + 8);									\
			strncpy(xx[i].value.str, r_buf, r_len);									\
			xx[i].value.str[r_len] = 0;												\
		} else {																	\
			xx[i].value.str = NULL;													\
		}																			\
	} else {																		\
		f_stat = fscanf(X32File, "%d ", &xx[i].value.ii);							\
	}

int X32Init() {
	int i, j, k, type, f_stat;
	X32command* Xarray;
	FILE *X32File;
//
	if ((X32File = fopen(".X32res.rc", "r")) == NULL) return 1;
	printf("Reading init file...");
	fflush(stdout);
// read file init values for all data
	for (i = 0; i < Xconfig_max; i++) {
		restore(Xconfig)
	}
	for (i = 0; i < Xmain_max; i++) {
		restore(Xmain);
	}
	for (i = 0; i < Xprefs_max; i++) {
		restore(Xprefs);
	}
	for (i = 0; i < Xstat_max; i++) {
		restore(Xstat);
	}
	Xarray = Xchannelset[0];
	for (i = 0; i < Xchannel01_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 32; j++) {
		Xarray = Xchannelset[j];
		for (i = 0; i < Xchannel02_max; i++) {
			restore(Xarray);
		}
	}
	Xarray = Xauxinset[0];
	for (i = 0; i < Xauxin01_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 8; j++) {
		Xarray = Xauxinset[j];
		for (i = 0; i < Xauxin02_max; i++) {
			restore(Xarray);
		}
	}
	Xarray = Xfxrtnset[0];
	for (i = 0; i < Xfxrtn01_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 8; j++) {
		Xarray = Xfxrtnset[j];
		for (i = 0; i < Xfxrtn02_max; i++) {
			restore(Xarray);
		}
	}
	Xarray = Xbusset[0];
	for (i = 0; i < Xbus01_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 16; j++) {
		Xarray = Xbusset[j];
		for (i = 0; i < Xbus02_max; i++) {
			restore(Xarray);
		}
	}
	Xarray = Xmtxset[0];
	for (i = 0; i < Xmtx01_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 6; j++) {
		Xarray = Xmtxset[j];
		for (i = 0; i < Xmtx02_max; i++) {
			restore(Xarray);
		}
	}
	for (i = 0; i < Xdca_max; i++) {
		restore(Xdca);
	}
	Xarray = Xfxset[0];
	for (i = 0; i < Xfx1_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 8; j++) {
		Xarray = Xfxset[j];
		if (j < 4) {
			for (i = 0; i < Xfx2_max; i++) {
				restore(Xarray);
			}
		} else {
			for (i = 0; i < Xfx5_max; i++) {
				restore(Xarray);
			}
		}
	}
	for (i = 0; i < Xoutput_max; i++) {
		restore(Xoutput);
	}
	Xarray = Xheadmpset[0];
	for (i = 0; i < Xheadamp_max; i++) {
		restore(Xarray);
	}
	for (j = 1; j < 128; j++) {
		Xarray = Xheadmpset[j];
		for (i = 0; i < Xheadamp1_max; i++) {
			restore(Xarray);
		}
	}
	for (i = 0; i < Xmisc_max; i++) {
		restore(Xmisc);
	}
	for (i = 0; i < Xurec_max; i++) {
		restore(Xurec);
	}
	for (i = 0; i < Xlibsc_max; i++) {
		restore(Xlibsc);
	}
	for (i = 0; i < Xlibsr_max; i++) {
		restore(Xlibsr);
	}
	for (i = 0; i < Xlibsf_max; i++) {
		restore(Xlibsf);
	}
	i = f_stat; // to avoid gcc warning;
	fclose(X32File);
	printf(" Done\n");
	fflush(stdout);
	return 0;
}



void Xdump(char *buf, int len, int debug)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;
	union littlebig {
		char	c1[4];
		short	si[2];
		int		i1;
		float	f1;
	} endian5;

	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		if (debug) printf(" %c", c);
		else printf("%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					if (debug) printf(" ~");
					else printf("~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*)(buf+data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char)buf[data++];
							if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
							if (debug) printf(" %c", c);
							else printf("%c", c);
						}
					}
//						printf("str: %s ",((char*)(buf+data)));
//						data += (strlen((char*)(buf+data)) + 4) & ~3;
					break;
				case 'i':
					for (k = 4; k > 0; endian5.c1[--k] = buf[data++]);
					printf("[%6d]", endian5.i1);
					break;
				case 'f':
					for (k = 4; k > 0; endian5.c1[--k] = buf[data++]);
					if (endian5.f1 < 10.) printf("[%06.4f]", endian5.f1);
					else if (endian5.f1 < 100.) printf("[%06.3f]", endian5.f1);
					else if (endian5.f1 < 1000.) printf("[%06.2f]", endian5.f1);
					else if (endian5.f1 < 10000.) printf("[%06.1f]", endian5.f1);
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian5.c1[--k] = buf[data++]);
					n = endian5.i1;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian5.c1[k++] = buf[data++]);
					// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (n == endian5.i1) {
						// Display blob as string
						printf("%d chrs: ", n);
						for (j = 0; j < n; j++) printf("%c ", buf[data++]);
					} else {
						// Display blob depending on command (as floats most of the time)
						if(strncmp(buf, "/meters/15", 10) == 0) {
							n = endian5.i1 * 2;
							printf("%d rta: \n", n);
							for (j = 0; j < n; j++) {
								//data as short ints, little-endian format
								for (k = 0; k < 2; endian5.c1[k++] = buf[data++]);
								endian5.f1 = (float)endian5.si[0] / 256.0;
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								printf("[%d] %07.2f ", j, endian5.f1);
							}
						} else if(strncmp(buf, "/meters/16", 10) == 0) {
							n = endian5.i1 * 2;
							printf("M/16: %d shorts\n", n);
							for (j = 0; j < n - 8; j++) {
								//printf("%02x %02x, ", (unsigned char)buf[data], (unsigned char)buf[data+1]);
								for (k = 0; k < 2; endian5.c1[k++] = buf[data++]);
								endian5.f1 = (float)endian5.si[0] / 32767.0;
								if (j < 32) printf("[%d: G %07.2f] ", j, endian5.f1);
								if (j < 64) printf("[%d: C %07.2f] ", j, endian5.f1);
								if (j < 80) printf("[%d: B %07.2f] ", j, endian5.f1);
								if (j < 86) printf("[%d: M %07.2f] ", j, endian5.f1);
								if (j == 86) printf("[%d:LR %07.2f] ", j, endian5.f1);
								if (j == 87) printf("[%d:MC %07.2f] ", j, endian5.f1);
							}
							for (; j < n; j++) {
								//printf("%02x %02x, ", (unsigned char)buf[data], (unsigned char)buf[data+1]);
								for (k = 0; k < 2; endian5.c1[k++] = buf[data++]);
								endian5.f1 = (float)endian5.si[0] / 256.0;
								printf("[%d: A %07.2f] ", j, endian5.f1);
							}
							printf("\n");

						} else {
							n = endian5.i1;
							printf("%d flts: ", n);
							for (j = 0; j < n; j++) {
								//floats are little-endian format
								for (k = 0; k < 4; endian5.c1[k++] = buf[data++]);
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								printf("%06.2f ", endian5.f1);
							}
						}
					}
					break;
				default:
					break;
				}
			}
			i = data - 1;
		}
	}
	printf("\n");
}



void Xfdump(char *header, char *buf, int len, int debug) {
int i;

if (debug) {
		printf("%s, %4d B: ", header, len);
		for (i = 0; i < len; i++) {
			printf("%02x", (unsigned char)buf[i]);
		}
		printf("\n");
	}
	if (debug) for (i = 0; i < (strlen(header) + 10); i++) printf(" ");
	else       printf("%s, %4d B: ", header, len);
	Xdump(buf, len, debug);
}

void Xsdump(char *str_out, char *buf, int len)
{
	int i, k, n, j, l, comma = 0, data = 0, dtc = 0;
	unsigned char c;
	char *out;
	union littlebig {
		char	c1[4];
		short	si[2];
		int		i1;
		float	f1;
	} endian;

	out = str_out;
	for (i = 0; i < len; i++) {
		c = (unsigned char)buf[i];
		if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
		sprintf(out++, "%c", c);
		if (c == ',') {
			comma = i;
			dtc = 1;
		}
		if (dtc && (buf[i] == 0)) {
			data = (i + 4) & ~3;
			for (dtc = i + 1; dtc < data ; dtc++) {
				if (dtc < len) {
					sprintf(out++, "~");
				}
			}
			dtc = 0;
			l = data;
			while (++comma < l && data < len) {
				switch (buf[comma]) {
				case 's':
					k = (strlen((char*)(buf+data)) + 4) & ~3;
					for (j = 0; j < k; j++) {
						if (data < len) {
							c = (unsigned char)buf[data++];
							if (c < 32 || c == 127 || c == 255 ) c = '~'; // Manage unprintable chars
							sprintf(out++, "%c", c);
						}
					}
//						printf("str: %s ",((char*)(buf+data)));
//						data += (strlen((char*)(buf+data)) + 4) & ~3;
					break;
				case 'i':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					sprintf(out, "[%6d]", endian.i1);
					out += 8;
					break;
				case 'f':
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (endian.f1 < 10.) sprintf(out, "[%06.4f]", endian.f1);
					else if (endian.f1 < 100.) sprintf(out, "[%06.3f]", endian.f1);
					else if (endian.f1 < 1000.) sprintf(out, "[%06.2f]", endian.f1);
					else if (endian.f1 < 10000.) sprintf(out, "[%06.1f]", endian.f1);
					out += 8;
					break;
				case 'b':
					// Get the number of bytes
					for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					n = endian.i1;
					// Get the number of data (floats or ints ???) in little-endian format
					for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
					// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
					if (n == endian.i1) {
						// Display blob as string
						sprintf(out, "%3d chrs: ", n);
						out += 10;
						for (j = 0; j < n; j++) sprintf(out++, "%c ", buf[data++]);
					} else {
						// Display blob depending on command (as floats most of the time)
						if(strncmp(buf, "/meters/15", 10) == 0) {
							n = endian.i1 * 2;
							sprintf(out, "%3d rta: \n", n);
							out += 9;
							for (j = 0; j < n; j++) {
								//data as short ints, little-endian format
								for (k = 0; k < 2; endian.c1[k++] = buf[data++]);
								endian.f1 = (float)endian.si[0] / 256.0;
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								sprintf(out, "%07.2f ", endian.f1);
								out += 8;
							}
						} else {
							n = endian.i1;
							sprintf(out, "%3d flts: ", n);
							out += 10;
							for (j = 0; j < n; j++) {
								//floats are little-endian format
								for (k = 0; k < 4; endian.c1[k++] = buf[data++]);
								// for (k = 4; k > 0; endian.c1[--k] = buf[data++]);
								sprintf(out, "%06.2f ", endian.f1);
								out += 7;
							}
						}
					}
					break;
				default:
					break;
				}
			}
			i = data - 1;
		}
	}
	sprintf(out++, "\n");
	*out++ = 0;
}

int Xsprint(char *bd, int index, char format, void *bs)
{
	int i;
// check format
	switch (format) {
	case 's':
	// string : copy characters one at a time until a 0 is found
		if (bs) {
			strcpy (bd+index, bs);
			index += (int)strlen(bs) + 1;
		} else {
			bd[index++] = 0;
		}
	// align to 4 bytes boundary if needed
		while (index & 3) bd[index++] = 0;
		break;
	case 'f':
	case 'i':
	// float or int : copy the 4 bytes of float or int in big-endian order
		i = 4;
		while (i > 0)
			bd[index++] = (char)(((char*)bs)[--i]);
		break;
	default:
	// don't copy anything
		break;
	}
	return index;
}


int Xfprint(char *bd, int index, char* text, char format, void *bs)
{
// first copy text
	strcpy (bd+index, text);
	index += (int)strlen(text) + 1;
// align to 4 bytes boundary if needed
	while (index & 3) bd[index++] = 0;
// then set format, keeping #4 alignment
	bd[index++] = ',';
	bd[index++] = format;
	bd[index++] = 0;
	bd[index++] = 0;
// based on format, set value
	return Xsprint(bd, index, format, bs);
}

