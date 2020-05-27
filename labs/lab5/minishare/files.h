/*
MiniShare - Minimal HTTP Server
Copyright (C) 2002-2004  Tero Lindeman

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef ___FILES_H
#define ___FILES_H

#include <windows.h>

typedef struct {
	char *filename;
	char *shared_as;
	int visible,auth; // auth = login to use, -1 = none
	__int64 filesize;
	char filedate[64],mime[64];
} SHAREDFILE;

void gettimestamp(char *fn,char *date);
char * getmimetype(char *fn);
__int64 get_filesize(char *fn);
void saveurls();
void loadurls();
void getsharedurl(char *fn,char *url,int rnd);
void geturlforid(int i,char * url);
void deletesharedfile(int i,int s);
int addsharedfile(char *fn);
void deletesharedfilebyfilename(char*fn);
void deletesharedfilebyurl(char*url);
int findsharedfilebyurl(char *url);

#endif
