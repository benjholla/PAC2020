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
#include "error.h"
#include "server.h"
#include "minishare.h"

#define ERRORS 10

static const error_message message[ERRORS]={
	{400,"Bad request",""},
	{401,"Unauthorized","WWW-Authenticate: Basic realm=\"MiniShare\"\r\n"},
	{403,"Forbidden",""},
	{404,"File not found",""},
	{405,"Method not allowed","Allow: GET, HEAD"},
	{416,"Range not satisfiable",""},
	{500,"Internal server error",""},
	{501,"Not implemented",""},

	{503,"Service unavailable",""},
	{505,"HTTP version not supported",""}
};

void send_error(SOCKET s,connection *c,int code) {
	char page[8000],err[80],header[800];
	int i;
	for (i=0;i<ERRORS && message[i].code!=code;i++);
	if (i<ERRORS) {
		strcpy(header,message[i].header);
		sprintf(err,"%d %s",message[i].code,message[i].message);
	} else {
		strcpy(header,"");
		sprintf(err,"%d Error",code);

	}
	sprintf(page,"HTTP/1.1 %s\r\n%sContent-Type: text/html\r\n\r\n"DOCTYPE"\r\n<html><head>"LINKCSS"<title>%s</title></head><body><h1>%s</h1></body>",err,header,err,err);
	sendmsg(s,page,c);
	send_sig(s,c);
	addtolog(c->con_id,err);
}

