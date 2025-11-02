/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "X32.h"


void X32::construct_xmain(){
    uint16_t i=0;

	Xmain[i++] = new X32command("/main",							{BSCO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st",						{BSCO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/config",					{BSCO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/config/name",		{S32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/config/icon",		{I32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/config/color",		{E32}, F_XET, {0}, Xcolors);
    Xmain[i++] = new X32command("/main/st/dyn",					{MXDY}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/on",				{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/dyn/mode",			{E32}, F_XET, {0}, Xdymode);
    Xmain[i++] = new X32command("/main/st/dyn/det",			{E32}, F_XET, {0}, Xdydet);
    Xmain[i++] = new X32command("/main/st/dyn/env",			{E32}, F_XET, {0}, Xdyenv);
    Xmain[i++] = new X32command("/main/st/dyn/thr",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/ratio",			{E32}, F_XET, {0}, Xdyrat);
    Xmain[i++] = new X32command("/main/st/dyn/knee",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/mgain",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/attack",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/hold",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/release",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/pos",			{E32}, F_XET, {0}, Xdyppos);
    Xmain[i++] = new X32command("/main/st/dyn/mix",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/auto",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/dyn/filter",				{CHDF}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/dyn/filter/on",		{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/dyn/filter/type",	{E32}, F_XET, {0}, Xdyftyp);
    Xmain[i++] = new X32command("/main/st/dyn/filter/f",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/insert",					{CHIN}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/insert/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/insert/pos",			{E32}, F_XET, {0}, Xdyppos);
    Xmain[i++] = new X32command("/main/st/insert/sel",			{E32}, F_XET, {0}, Xisel);
    Xmain[i++] = new X32command("/main/st/eq",						{OffOn}, F_FND, {1}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/on",				{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/eq/1",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/1/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/st/eq/1/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/1/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/1/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/2",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/2/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/st/eq/2/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/2/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/2/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/3",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/3/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/st/eq/3/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/3/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/3/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/4",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/4/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/st/eq/4/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/4/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/4/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/5",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/5/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/st/eq/5/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/5/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/5/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/6",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/6/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/st/eq/6/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/6/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/eq/6/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix",					{MSMX}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/on",				{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/fader",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/01",					{CHMO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/01/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/01/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/01/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/01/type",		{E32}, F_XET, {0}, Xmtype);
    Xmain[i++] = new X32command("/main/st/mix/02",					{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/02/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/02/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/03",					{CHMO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/03/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/03/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/03/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/03/type",		{E32}, F_XET, {0}, Xmtype);
    Xmain[i++] = new X32command("/main/st/mix/04",					{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/04/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/04/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/05",					{CHMO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/05/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/05/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/05/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/05/type",		{E32}, F_XET, {0}, Xmtype);
    Xmain[i++] = new X32command("/main/st/mix/06",					{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/mix/06/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/st/mix/06/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/grp",					{CHGRP}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/grp/dca",			{P32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/st/grp/mute",			{P32}, F_XET, {0}, NULL);

    Xmain[i++] = new X32command("/main/m",							{BSCO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/config",					{BSCO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/config/name",			{S32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/config/icon",			{I32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/config/color",		{E32}, F_XET, {0}, Xcolors);
    Xmain[i++] = new X32command("/main/m/dyn",						{MXDY}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/on",				{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/dyn/mode",			{E32}, F_XET, {0}, Xdymode);
    Xmain[i++] = new X32command("/main/m/dyn/det",				{E32}, F_XET, {0}, Xdydet);
    Xmain[i++] = new X32command("/main/m/dyn/env",				{E32}, F_XET, {0}, Xdyenv);
    Xmain[i++] = new X32command("/main/m/dyn/thr",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/ratio",			{E32}, F_XET, {0}, Xdyrat);
    Xmain[i++] = new X32command("/main/m/dyn/knee",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/mgain",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/attack",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/hold",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/release",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/pos",				{E32}, F_XET, {0}, Xdyppos);
    Xmain[i++] = new X32command("/main/m/dyn/mix",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/auto",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/dyn/filter",				{CHDF}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/dyn/filter/on",		{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/dyn/filter/type",		{E32}, F_XET, {0}, Xdyftyp);
    Xmain[i++] = new X32command("/main/m/dyn/filter/f",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/insert",					{CHIN}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/insert/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/insert/pos",			{E32}, F_XET, {0}, Xdyppos);
    Xmain[i++] = new X32command("/main/m/insert/sel",			{E32}, F_XET, {0}, Xisel);
    Xmain[i++] = new X32command("/main/m/eq",						{OffOn}, F_FND, {1}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/on",				{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/eq/1",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/1/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/m/eq/1/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/1/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/1/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/2",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/2/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/m/eq/2/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/2/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/2/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/3",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/3/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/m/eq/3/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/3/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/3/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/4",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/4/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/m/eq/4/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/4/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/4/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/5",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/5/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/m/eq/5/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/5/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/5/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/6",					{CHEQ}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/6/type",			{E32}, F_XET, {0}, Xeqty2);
    Xmain[i++] = new X32command("/main/m/eq/6/f",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/6/g",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/eq/6/q",				{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix",						{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/on",				{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/fader",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/01",					{CHMO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/01/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/01/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/01/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/01/type",			{E32}, F_XET, {0}, Xmtype);
    Xmain[i++] = new X32command("/main/m/mix/02",					{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/02/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/02/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/03",					{CHMO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/03/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/03/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/03/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/03/type",			{E32}, F_XET, {0}, Xmtype);
    Xmain[i++] = new X32command("/main/m/mix/04",					{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/04/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/04/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/05",					{CHMO}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/05/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/05/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/05/pan",			{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/05/type",			{E32}, F_XET, {0}, Xmtype);
    Xmain[i++] = new X32command("/main/m/mix/06",					{CHME}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/mix/06/on",			{E32}, F_XET, {0}, OffOn);
    Xmain[i++] = new X32command("/main/m/mix/06/level",		{F32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/grp",						{CHGRP}, F_FND, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/grp/dca",				{P32}, F_XET, {0}, NULL);
    Xmain[i++] = new X32command("/main/m/grp/mute",			{P32}, F_XET, {0}, NULL);
}