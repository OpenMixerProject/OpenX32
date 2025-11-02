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


void X32::construct_xmisc(){
    uint16_t i=0;

    Xnode[i++] = new X32node("conf", 4, Xconfig, sizeof(Xconfig)/sizeof(X32command));
    Xnode[i++] = new X32node("main", 4, Xmain, sizeof(Xmain)/sizeof(X32command));
    Xnode[i++] = new X32node("-pre", 4, Xprefs, sizeof(Xprefs)/sizeof(X32command));
    Xnode[i++] = new X32node("-sta", 4, Xstat, sizeof(Xstat)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/01", 5, Xchannel01, sizeof(Xchannel01)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/02", 5, Xchannel02, sizeof(Xchannel02)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/03", 5, Xchannel03, sizeof(Xchannel03)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/04", 5, Xchannel04, sizeof(Xchannel04)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/05", 5, Xchannel05, sizeof(Xchannel05)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/06", 5, Xchannel06, sizeof(Xchannel06)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/07", 5, Xchannel07, sizeof(Xchannel07)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/08", 5, Xchannel08, sizeof(Xchannel08)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/09", 5, Xchannel09, sizeof(Xchannel09)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/10", 5, Xchannel10, sizeof(Xchannel10)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/11", 5, Xchannel11, sizeof(Xchannel11)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/12", 5, Xchannel12, sizeof(Xchannel12)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/13", 5, Xchannel13, sizeof(Xchannel13)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/14", 5, Xchannel14, sizeof(Xchannel14)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/15", 5, Xchannel15, sizeof(Xchannel15)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/16", 5, Xchannel16, sizeof(Xchannel16)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/17", 5, Xchannel17, sizeof(Xchannel17)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/18", 5, Xchannel18, sizeof(Xchannel18)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/19", 5, Xchannel19, sizeof(Xchannel19)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/20", 5, Xchannel20, sizeof(Xchannel20)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/21", 5, Xchannel21, sizeof(Xchannel21)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/22", 5, Xchannel22, sizeof(Xchannel22)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/23", 5, Xchannel23, sizeof(Xchannel23)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/24", 5, Xchannel24, sizeof(Xchannel24)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/25", 5, Xchannel25, sizeof(Xchannel25)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/26", 5, Xchannel26, sizeof(Xchannel26)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/27", 5, Xchannel27, sizeof(Xchannel27)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/28", 5, Xchannel28, sizeof(Xchannel28)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/29", 5, Xchannel29, sizeof(Xchannel29)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/30", 5, Xchannel30, sizeof(Xchannel30)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/31", 5, Xchannel31, sizeof(Xchannel31)/sizeof(X32command));
    Xnode[i++] = new X32node("ch/32", 5, Xchannel32, sizeof(Xchannel32)/sizeof(X32command));
    Xnode[i++] = new X32node("ch", 2, Xchannel01, sizeof(Xchannel01)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/01", 8, Xauxin01, sizeof(Xauxin01)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/02", 8, Xauxin02, sizeof(Xauxin02)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/03", 8, Xauxin03, sizeof(Xauxin03)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/04", 8, Xauxin04, sizeof(Xauxin04)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/05", 8, Xauxin05, sizeof(Xauxin05)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/06", 8, Xauxin06, sizeof(Xauxin06)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/07", 8, Xauxin07, sizeof(Xauxin07)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin/08", 8, Xauxin08, sizeof(Xauxin08)/sizeof(X32command));
    Xnode[i++] = new X32node("auxin", 5, Xauxin01, sizeof(Xauxin01)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/01", 8, Xfxrtn01, sizeof(Xfxrtn01)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/02", 8, Xfxrtn02, sizeof(Xfxrtn02)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/03", 8, Xfxrtn03, sizeof(Xfxrtn03)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/04", 8, Xfxrtn04, sizeof(Xfxrtn04)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/05", 8, Xfxrtn05, sizeof(Xfxrtn05)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/06", 8, Xfxrtn06, sizeof(Xfxrtn06)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/07", 8, Xfxrtn07, sizeof(Xfxrtn07)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn/08", 8, Xfxrtn08, sizeof(Xfxrtn08)/sizeof(X32command));
    Xnode[i++] = new X32node("fxrtn", 5, Xfxrtn01, sizeof(Xfxrtn01)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/1", 4, Xfx1, sizeof(Xfx1)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/2", 4, Xfx2, sizeof(Xfx2)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/3", 4, Xfx3, sizeof(Xfx3)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/4", 4, Xfx4, sizeof(Xfx4)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/5", 4, Xfx5, sizeof(Xfx5)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/6", 4, Xfx6, sizeof(Xfx6)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/7", 4, Xfx7, sizeof(Xfx7)/sizeof(X32command));
    Xnode[i++] = new X32node("fx/8", 4, Xfx8, sizeof(Xfx8)/sizeof(X32command));
    Xnode[i++] = new X32node("fx", 2, Xfx1, sizeof(Xfx1)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/01", 6, Xbus01, sizeof(Xbus01)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/02", 6, Xbus02, sizeof(Xbus02)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/03", 6, Xbus03, sizeof(Xbus03)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/04", 6, Xbus04, sizeof(Xbus04)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/05", 6, Xbus05, sizeof(Xbus05)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/06", 6, Xbus06, sizeof(Xbus06)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/07", 6, Xbus07, sizeof(Xbus07)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/08", 6, Xbus08, sizeof(Xbus08)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/09", 6, Xbus09, sizeof(Xbus09)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/10", 6, Xbus10, sizeof(Xbus10)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/11", 6, Xbus11, sizeof(Xbus11)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/12", 6, Xbus12, sizeof(Xbus12)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/13", 6, Xbus13, sizeof(Xbus13)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/14", 6, Xbus14, sizeof(Xbus14)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/15", 6, Xbus15, sizeof(Xbus15)/sizeof(X32command));
    Xnode[i++] = new X32node("bus/16", 6, Xbus16, sizeof(Xbus16)/sizeof(X32command));
    Xnode[i++] = new X32node("bus", 3, Xbus01, sizeof(Xbus01)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx/01", 6, Xmtx01, sizeof(Xmtx01)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx/02", 6, Xmtx02, sizeof(Xmtx02)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx/03", 6, Xmtx03, sizeof(Xmtx03)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx/04", 6, Xmtx04, sizeof(Xmtx04)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx/05", 6, Xmtx05, sizeof(Xmtx05)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx/06", 6, Xmtx06, sizeof(Xmtx06)/sizeof(X32command));
    Xnode[i++] = new X32node("mtx", 3, Xmtx01, sizeof(Xmtx01)/sizeof(X32command));
    Xnode[i++] = new X32node("dca", 3, Xdca, sizeof(Xdca)/sizeof(X32command));
    Xnode[i++] = new X32node("outputs/main/01", 8, Xoutput, sizeof(Xoutput)/sizeof(X32command));
    Xnode[i++] = new X32node("outputs/main", 8, Xoutput, sizeof(Xoutput)/sizeof(X32command));
    Xnode[i++] = new X32node("outputs", 8, Xoutput, sizeof(Xoutput)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/000", 11, Xheadamp, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/001", 11, Xheadamp001, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/002", 11, Xheadamp002, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/003", 11, Xheadamp003, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/004", 11, Xheadamp004, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/005", 11, Xheadamp005, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/006", 11, Xheadamp006, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/007", 11, Xheadamp007, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/008", 11, Xheadamp008, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/009", 11, Xheadamp009, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/010", 11, Xheadamp010, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/011", 11, Xheadamp011, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/012", 11, Xheadamp012, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/013", 11, Xheadamp013, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/014", 11, Xheadamp014, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/015", 11, Xheadamp015, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/016", 11, Xheadamp016, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/017", 11, Xheadamp017, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/018", 11, Xheadamp018, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/019", 11, Xheadamp019, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/020", 11, Xheadamp020, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/021", 11, Xheadamp021, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/022", 11, Xheadamp022, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/023", 11, Xheadamp023, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/024", 11, Xheadamp024, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/025", 11, Xheadamp025, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/026", 11, Xheadamp026, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/027", 11, Xheadamp027, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/028", 11, Xheadamp028, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/029", 11, Xheadamp029, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/030", 11, Xheadamp030, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/031", 11, Xheadamp031, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/032", 11, Xheadamp032, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/033", 11, Xheadamp033, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/034", 11, Xheadamp034, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/035", 11, Xheadamp035, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/036", 11, Xheadamp036, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/037", 11, Xheadamp037, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/038", 11, Xheadamp038, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/039", 11, Xheadamp039, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/040", 11, Xheadamp040, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/041", 11, Xheadamp041, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/042", 11, Xheadamp042, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/043", 11, Xheadamp043, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/044", 11, Xheadamp044, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/045", 11, Xheadamp045, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/046", 11, Xheadamp046, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/047", 11, Xheadamp047, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/048", 11, Xheadamp048, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/049", 11, Xheadamp049, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/050", 11, Xheadamp050, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/051", 11, Xheadamp051, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/052", 11, Xheadamp052, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/053", 11, Xheadamp053, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/054", 11, Xheadamp054, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/055", 11, Xheadamp055, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/056", 11, Xheadamp056, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/057", 11, Xheadamp057, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/058", 11, Xheadamp058, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/059", 11, Xheadamp059, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/060", 11, Xheadamp060, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/061", 11, Xheadamp061, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/062", 11, Xheadamp062, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/063", 11, Xheadamp063, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/064", 11, Xheadamp064, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/065", 11, Xheadamp065, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/066", 11, Xheadamp066, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/067", 11, Xheadamp067, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/068", 11, Xheadamp068, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/069", 11, Xheadamp069, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/070", 11, Xheadamp070, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/071", 11, Xheadamp071, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/072", 11, Xheadamp072, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/073", 11, Xheadamp073, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/074", 11, Xheadamp074, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/075", 11, Xheadamp075, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/076", 11, Xheadamp076, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/077", 11, Xheadamp077, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/078", 11, Xheadamp078, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/079", 11, Xheadamp079, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/080", 11, Xheadamp080, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/081", 11, Xheadamp081, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/082", 11, Xheadamp082, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/083", 11, Xheadamp083, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/084", 11, Xheadamp084, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/085", 11, Xheadamp085, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/086", 11, Xheadamp086, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/087", 11, Xheadamp087, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/088", 11, Xheadamp088, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/089", 11, Xheadamp089, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/090", 11, Xheadamp090, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/091", 11, Xheadamp091, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/092", 11, Xheadamp092, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/093", 11, Xheadamp093, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/094", 11, Xheadamp094, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/095", 11, Xheadamp095, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/096", 11, Xheadamp096, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/097", 11, Xheadamp097, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/098", 11, Xheadamp098, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/099", 11, Xheadamp099, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/100", 11, Xheadamp100, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/101", 11, Xheadamp101, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/102", 11, Xheadamp102, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/103", 11, Xheadamp103, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/104", 11, Xheadamp104, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/105", 11, Xheadamp105, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/106", 11, Xheadamp106, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/107", 11, Xheadamp107, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/108", 11, Xheadamp108, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/109", 11, Xheadamp109, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/110", 11, Xheadamp110, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/111", 11, Xheadamp111, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/112", 11, Xheadamp112, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/113", 11, Xheadamp113, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/114", 11, Xheadamp114, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/115", 11, Xheadamp115, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/116", 11, Xheadamp116, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/117", 11, Xheadamp117, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/118", 11, Xheadamp118, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/119", 11, Xheadamp119, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/120", 11, Xheadamp120, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/121", 11, Xheadamp121, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/122", 11, Xheadamp122, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/123", 11, Xheadamp123, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/124", 11, Xheadamp124, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/125", 11, Xheadamp125, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/126", 11, Xheadamp126, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp/127", 11, Xheadamp127, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("headamp", 7, Xheadamp, sizeof(Xheadamp)/sizeof(X32command));
    Xnode[i++] = new X32node("-ha", 3, Xmisc, size_Xmisc);
    Xnode[i++] = new X32node("-usb", 4, Xmisc, size_Xmisc);
    Xnode[i++] = new X32node("undo", 4, Xdummy, sizeof(Xdummy)/sizeof(X32command));
    Xnode[i++] = new X32node("-action", 7, Xdummy, sizeof(Xdummy)/sizeof(X32command));
    Xnode[i++] = new X32node("-show/showfile/snippet", 22, Xsnippet, sizeof(Xsnippet)/sizeof(X32command));	// !! keep
    Xnode[i++] = new X32node("-show/showfile/scene", 20, Xscene, sizeof(Xscene)/sizeof(X32command)); 		// in this
    Xnode[i++] = new X32node("-show", 5, Xshow, sizeof(Xshow)/sizeof(X32command));							// order !!
    Xnode[i++] = new X32node("-urec", 5, Xurec, sizeof(Xurec)/sizeof(X32command));
    Xnode[i++] = new X32node("-libs/fx", 8, Xlibsf, sizeof(Xlibsf)/sizeof(X32command));					// !! keep
    Xnode[i++] = new X32node("-libs/r", 7, Xlibsr, sizeof(Xlibsr)/sizeof(X32command));						// in this
    Xnode[i++] = new X32node("-libs", 5, Xlibsc, sizeof(Xlibsc)/sizeof(X32command));						// order !!
}