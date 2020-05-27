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

#include "serve2.h"
#include "globals.h"
#include "files.h"
#include "error.h"
#include <winsock2.h>

typedef struct {
	int id;
	char *name;
} field_id;

#define HEADERFIELDS 3

static const field_id field[HEADERFIELDS]= {
	{REQ_HEAD_RANGE,"Range"},
	{REQ_HEAD_CONNECTION,"Connection"},
	{REQ_HEAD_AUTH,"Authorization"}
};

int get_header_field(char *ln,char *value) {
	int i;
	char tmp[strlen(ln)+100];
	sscanf(ln,"%[^:]: %[^\r\n]",tmp,value);
	for (i=0;i<HEADERFIELDS;i++) {
		//addtolog(0,"FIELD:")	;
		//char tt[800];
		//sprintf(tt,"'%s' '%s'",tmp,field[i].name);
		//addtolog(0,tt)	;
		if (strcmp(tmp,field[i].name)==0) {


			return field[i].id;
		}
	}
	return REQ_HEAD_NONE;
}

int send_response_header(connection *c,http_request *req) {
	char header[MAXHEADERSIZE+1];



	if ((req->type&REQ_PARTIAL)!=0) {
		sprintf(header,"HTTP/1.1 206 Partial Content\r\nContent-Range: bytes %I64d-%I64d/%I64d\r\n",c->range_start,c->range_end,sharedfiles[c->file].filesize);
		sendmsg(c->socket,header,c);
		addtolog(c->con_id,"206 Partial Content");
	} else {
		sendmsg(c->socket,"HTTP/1.1 200 OK\r\n",c);
		addtolog(c->con_id,"200 OK");
	}

	char fd[1000];
	if (c->file!=-1) strcpy(fd,sharedfiles[c->file].filedate); else gettimestamp(css_path,fd);
	sprintf(header,"Accept-Ranges: bytes\r\nContent-Length: %I64d\r\nContent-Type: %s\r\nLast-Modified: %s\r\nConnection: %s\r\n\r\n",
		c->range_end-c->range_start+1,getmimetype(c->request),fd,req->keepalive==0?"close":"Keep-Alive");
	sendmsg(c->socket,header,c);
	return 1;
}

int send_file_rangeless(connection *c,char *fn){
	c->range_start=0;
	c->range_end=get_filesize(fn);
	return  send_file(c,fn);
}

int send_file(connection *c,char *fn){

	FILE *f;
	f=fopen(fn,"rb");
	if (f==NULL) {
		char tmp[800];
		sprintf(tmp,"Can't open '%s'.",fn);
		addtolog(c->con_id,tmp);
		return 0;
	}
	fseek(f,c->range_start,SEEK_SET);

	int finished=0,fpos=ftell(f),b=0;
	while (!finished) {
		char buffer[SEND_BUFFER_SIZE+1];
		int r=fread(buffer,1,SEND_BUFFER_SIZE,f);
		if (fpos+r>c->range_end)  {
			r=c->range_end-fpos+1;
		}
		if (r==0) finished=1; else {
			int sent;
			if ((sent=send(c->socket,buffer,r,0))==SOCKET_ERROR) finished=1;
			else {
				c->bytesdownloaded+=sent;
			}
		}
		b=(b+1)%4; 
		if (b==0) SendMessage(hWnd,WM_USER+7,0,0);
	}

	fclose(f);
	return 1;
}


int get_request_header(SOCKET s,http_request *req,int con_id) {
	req->keepalive=0;
	char header[MAXHEADERSIZE+1];
	int hs=0,r=SOCKET_ERROR;
	do {
		r=recv(s,header+hs,MAXHEADERSIZE-hs,0);
		if (r==0 || r==WSAECONNRESET) return REQ_ERR_MALFORMED_REQUEST;
		hs+=r;
		if (hs>=MAXHEADERSIZE) return REQ_ERR_REQENTITYTOOLARGE;
		header[hs]='\0';
	} while (strcmp(header+hs-4,"\r\n\r\n")!=0 || r==SOCKET_ERROR);


	char *request_line=strtok(header,"\r");
	if (request_line==NULL) return REQ_ERR_MALFORMED_REQUEST;
	addtolog(con_id,request_line);


	char method[MAXHEADERSIZE],ver[MAXHEADERSIZE];
	if (sscanf(request_line,"%s %s HTTP/%s",method,req->path,ver)!=3)
		if (sscanf(request_line,"%s %[^?]?%s HTTP/%s",method,req->path,req->request_string,ver)!=4) return REQ_ERR_MALFORMED_REQUEST;
	//if (strcmp(ver,"1.1")!=0) return REQ_ERR_VERSIONNOTSUPPORTED;
	// ^-- should be handled with a 101 page
	if (strcmp(method,"GET")!=0 && strcmp(method,"HEAD")!=0) return REQ_ERR_METHODNOTSUPPORTED;
	req->type=(strcmp(method,"GET")==0)?REQ_GET:REQ_HEAD;

	char *header_line[MAXHEADERLINES];
	char *tmp;
	int h=0;
	do {
		tmp=strtok(NULL,"\r");
		if (tmp!=NULL) {
			header_line[h]=tmp+1;
			//addtolog(0,header_line[h]);
			h++;
		}
	} while (tmp!=NULL&&h<MAXHEADERLINES);

	req->auth=0;
	req->keepalive=0;

	int i;
	for (i=0;i<h-2;i++) {
		char param[MAXHEADERSIZE];
		switch (get_header_field(header_line[i],param)) {
			case REQ_HEAD_RANGE:
				//addtolog(0,header_line[i]);
				req->type|=REQ_PARTIAL;
				if (sscanf(param,"bytes=%I64d-%I64d",&req->range_start,&req->range_end)==2) break;
				if (sscanf(param,"bytes=%I64d-",&req->range_start)==1) {
					req->range_end=-1;
					break;
				}
				if (sscanf(param,"bytes=-%I64d",&req->range_end)==1) {
					req->range_start=-1;
					break;
				}
			break;
			case REQ_HEAD_AUTH:
				//addtolog(666,param);
				if (sscanf(param,"Basic %s",req->auth_hash)!=1) return REQ_ERR_MALFORMED_REQUEST;
				req->auth=1;
			break;
			case REQ_HEAD_CONNECTION:
				if (strcmp("close",param)==0) req->keepalive=0;
				if (strcmp("Keep-Alive",param)==0) req->keepalive=1;
			break;
			case REQ_HEAD_NONE:
			break;
			default:
				return REQ_ERR_MALFORMED_REQUEST;
			break;
		}
	}

	return REQ_ERR_SUCCESS;
}

void sendcss(SOCKET s,connection *c,http_request *r)
{
	r->range_start=0;
	r->range_end=get_filesize(css_path_f)-1;
	r->type&=~REQ_PARTIAL;
	c->file=-1;
	c->range_start=r->range_start;
	c->range_end=r->range_end;
	send_response_header(c,r);

	if ((r->type&REQ_GET)!=0) send_file(c,css_path_f);
}

void _cdecl serve2(void* _cons) {
	((connection*)_cons)->inuse=true;
	SOCKET s=((connection*)_cons)->socket;
	connection *cons=((connection*)_cons);

	SendMessage(hWnd,WM_USER+6,0,0);
    SendMessage(hWnd,WM_USER+7,0,0);

	http_request req;

	int err;

again:  // <- sue me, g++ doesn't allow labels just before a "} while (blah);"

		err=get_request_header(s,&req,cons->con_id);

		if (err==REQ_ERR_SUCCESS) {
			int f;
			strcpy(cons->request,req.path);
			if (strcmp(req.path,sharepage)==0)
			{
				sendlist(s,cons);
				goto finish;
			}
			if (strcmp(req.path,"/minishare.css")==0)
			{
				sendcss(s,cons,&req);
				goto finish;
			}
			f=findsharedfilebyurl(req.path);
			if ((f)==-1) send_error(s,cons,404); else {
				cons->file=f;
				gettimestamp(sharedfiles[f].filename,sharedfiles[f].filedate);
				if ((sharedfiles[f].filesize=get_filesize(sharedfiles[f].filename))==-1) {
					send_error(s,cons,404);
					goto finish;
				}

				if ((req.type&REQ_PARTIAL)!=0) {
					if (req.range_start==-1) {
						req.range_start=sharedfiles[f].filesize-req.range_end;
						req.range_end=sharedfiles[f].filesize-1;
					} else if (req.range_end==-1) {
						req.range_end=sharedfiles[f].filesize-1;
					}

					if (req.range_start<0 || req.range_end>=sharedfiles[f].filesize || req.range_start>req.range_end) {
						send_error(s,cons,416);
						goto finish;
					}
				} else {
					req.range_start=0;
					req.range_end=sharedfiles[f].filesize-1;
				}
				if (sharedfiles[f].auth!=-1) {
					if (!req.auth) {
						send_error(s,cons,401);
						goto finish;
					}
					if (!check_authority(sharedfiles[f].auth,req.auth_hash)) {
						send_error(s,cons,401);
						goto finish;
					}
				}
				cons->filesize=sharedfiles[f].filesize;
				cons->range_start=req.range_start;
				cons->range_end=req.range_end;
				send_response_header(cons,&req);
				if ((req.type&REQ_GET)!=0) {
					send_file(cons,sharedfiles[cons->file].filename);
				}
			}

		} else {
			send_error(s,cons,err);
		}

finish:
	if (req.keepalive) {
		addtolog(cons->con_id,"Reusing connection...");
		goto again;
	}

	shutdown(s,SD_BOTH);
    closesocket(s);
    connections--;

    char tmp[800];
    sprintf(tmp,"Total %I64d bytes sent.",((connection*)_cons)->bytesdownloaded);
    addtolog(cons->con_id,tmp);

    addtolog(cons->con_id,"Connection closed.");
	((connection*)_cons)->inuse=false;

	SendMessage(hWnd,WM_USER+6,0,0);
	SendMessage(hWnd,WM_USER+7,0,0);

}
