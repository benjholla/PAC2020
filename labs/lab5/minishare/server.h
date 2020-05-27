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

#ifndef ___SERVER_H
#define ___SERVER_H

#include <winsock.h>

typedef struct {
	SOCKET socket;
	char addr[513],addr_ip[513],request[513];
	int sessionstart,last_touch,file,filesize,con_id;
	__int64 range_start,range_end;
	__int64 bytesdownloaded;
	bool inuse;
	int listpos;
} connection;


void addtolog(int i,char *msg);
void stop_server();
void start_server();
void init_server();
void deinit_server();

int sendmsg(SOCKET s,char *msg,connection *c);
void send_sig(SOCKET s,connection *c);
void sendlist(SOCKET s, connection *c);
int kick_user(int id);

#endif
