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

#include <stdio.h>
#include <time.h>
#include <strings.h>
#include "ban.h"
#include <cstring>

banned_ip bans[MAXBANS];

int load_bans(char *fn) {
	memset(bans,0,sizeof(banned_ip)*MAXBANS);
	FILE *f=fopen(fn,"r");
	if (f==NULL) return 0;
	while (!feof(f)) {
		char buf[256];
		fgets(buf,254,f);
		char *c=buf;
		while (*c>=' ') c++;
		*c='\0';
		add_ban(buf,-1);
	}
	fclose(f);	
	return 1;
}

int add_ban(char *ip,int d) {
	int i;
	for(i=0;(i<MAXBANS) && bans[i].expires!=0;i++);
	if (i>=MAXBANS) return 0;
	strcpy(bans[i].address,ip);
	if (d==-1) bans[i].expires=-1; else bans[i].expires=time(NULL)+d;
	return 1;	
}

int check_banned(char *ip) {
	int i;
	for (i=0;i<MAXBANS;i++) {
		if ((bans[i].expires==-1||bans[i].expires>time(NULL)))
		{
			if (bans[i].address[strlen(bans[i].address)-1]=='*') { // i.e. * handled in the end of an address only 
				if (strnicmp(ip,bans[i].address,strlen(bans[i].address)-1)==0) return 1;
			} else {
				if (stricmp(ip,bans[i].address)==0) return 1;
			}
		} else {
			if (bans[i].expires<=time(NULL)) bans[i].expires=0;
		}
	}
	return 0;
}
