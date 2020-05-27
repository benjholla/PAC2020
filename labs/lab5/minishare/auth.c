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

#include "auth.h"
#include "minishare.h"
#include "globals.h"
#include "misc.h"
#include "server.h"
#include "base64.c"
#include <stdio.h>
#include <strings.h>

int tot_users=0;
user users[MAXUSERS];

int add_user(char *lp) {
	if (tot_users>=MAXUSERS) return 0;

	if (sscanf(lp,"%[^:]:%s",users[tot_users].login,users[tot_users].passwd)==2) {
		//addtolog(0,lp);
		//addtolog(0,users[tot_users].login);
		//addtolog(0,users[tot_users].passwd);
		base64((unsigned char*)users[tot_users].base64hash,(unsigned char*)lp,strlen(lp));
		//addtolog(0,users[tot_users].base64hash);
		tot_users++;
	} else return 0;
	return 1;
}

int check_authority(int user,char *hash) {
	if (user>=tot_users||user<0) return 0;
	//addtolog(0,users[user].base64hash);
	//addtolog(0,hash);
	if (strcmp(users[user].base64hash,hash)==0) return 1;
	return 0;
}

int find_user(char *login) {
	int i=0;
	while (strcmp(users[i].login,login)!=0 && i<tot_users) i++;
	if (i==tot_users) return -1;
	return i;
}

int delete_user(int i) {
	if (i>=tot_users||i<0) return 0;
	int u;
	tot_users--;
	for (u=i;u<tot_users;u++) {
		strcpy(users[u].login,users[u+1].login);
		strcpy(users[u].passwd,users[u+1].passwd);
	}
	for (u=0;u<shared_files;u++) if (sharedfiles[u].auth==i) sharedfiles[u].auth=-1; else if (sharedfiles[u].auth>i) sharedfiles[u].auth--;
	return 1;
}

int load_users() {
	tot_users=0;
	FILE *f=fopen(make_workpath_file("users.txt"),"r");
	if (f!=NULL) {
		while (!feof(f)) {
			char tmp[32*2+2];
			if (fscanf(f,"%s",tmp)==1) if (strlen(tmp)>2) add_user(tmp);
		}
		fclose(f);
	} else return 0;
	return 1;
}

int save_users() {
	FILE *f=fopen(make_workpath_file("users.txt"),"w");
	if (f!=NULL) {
		int i;
		for (i=0;i<tot_users;i++) fprintf(f,"%s:%s\n",users[i].login,users[i].passwd);
		fclose(f);
		return 1;
	}
	return 0;
}
