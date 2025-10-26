/*
 * X32Dca.h
 *
 *  Created on: 4 févr. 2015
 *      Author: patrick
 */

#ifndef X32DCA_H_
#define X32DCA_H_

X32command Xdca[] = {
		{"/dca",						{CHME}, F_FND, {0}, NULL},
		{"/dca/1",						{CHME}, F_FND, {0}, NULL},
		{"/dca/1/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/1/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/1/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/1/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/1/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/1/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/2",						{CHME}, F_FND, {0}, NULL},
		{"/dca/2/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/2/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/2/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/2/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/2/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/2/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/3",						{CHME}, F_FND, {0}, NULL},
		{"/dca/3/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/3/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/3/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/3/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/3/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/3/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/4",						{CHME}, F_FND, {0}, NULL},
		{"/dca/4/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/4/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/4/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/4/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/4/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/4/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/5",						{CHME}, F_FND, {0}, NULL},
		{"/dca/5/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/5/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/5/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/5/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/5/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/5/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/6",						{CHME}, F_FND, {0}, NULL},
		{"/dca/6/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/6/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/6/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/6/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/6/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/6/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/7",						{CHME}, F_FND, {0}, NULL},
		{"/dca/7/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/7/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/7/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/7/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/7/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/7/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/dca/8",						{CHME}, F_FND, {0}, NULL},
		{"/dca/8/on",				{E32}, F_XET, {0}, OffOn},
		{"/dca/8/fader",			{F32}, F_XET, {0}, NULL},
		{"/dca/8/config",				{BSCO}, F_FND, {0}, NULL},
		{"/dca/8/config/name",		{S32}, F_XET, {0}, NULL},
		{"/dca/8/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/dca/8/config/color",		{E32}, F_XET, {0}, Xcolors},
};
int Xdca_max = sizeof(Xdca) / sizeof(X32command); 

#endif /* X32DCA_H_ */
