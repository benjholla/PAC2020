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

#include "minishare.h"
#include <winsock2.h>
#include <stdio.h>
#include "auth.h"
#include "server.h"
#define MAXHEADERSIZE 8192
#define MAXHEADERLINES 128

#define REQ_GET 1
#define REQ_HEAD 2
#define REQ_PARTIAL 1024


#define REQ_HEAD_RANGE 1
#define REQ_HEAD_AUTH 2
#define REQ_HEAD_CONNECTION 3
#define REQ_HEAD_NONE 0

#define REQ_ERR_SUCCESS 0
#define REQ_ERR_MALFORMED_REQUEST 400
#define REQ_ERR_METHODNOTSUPPORTED 405
#define REQ_ERR_REQENTITYTOOLARGE 413
#define REQ_ERR_REQURITOOLONG 414
#define REQ_ERR_VERSIONNOTSUPPORTED 505

typedef struct http_request {
	int type,auth,keepalive;
	char path[MAXHEADERSIZE],request_string[MAXHEADERSIZE];
	char auth_hash[(MAXLPSIZE*2)*4/3+2];
	__int64 range_start,range_end;
} http_request;

int send_file(connection *c,char *fn);
int send_file_rangeless(connection *c,char *fn);
void _cdecl serve2(void* _cons);
