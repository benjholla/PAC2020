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
#include "globals.h"
#include "ipdiag.h"
#include "files.h"
#include "misc.h"
#include "server.h"
#include "serve.h"
#include "serve2.h"
#include "ban.h"
#include "error.h"

#include <stdio.h>
//#include <math.h>  // removed unused dependency on math.h (which has a build error)
#include <process.h>
#include <ctype.h>
#include <time.h>
#include <winsock2.h>
#include <io.h>
#include <fcntl.h>
#include <mswsock.h>


int old_engine=0;

int con_id;




SOCKET sock;
connection cons[256];



int sendmsg(SOCKET s,char *msg,connection *c) {
		int sent=send(s,msg,strlen(msg),0);
		c->bytesdownloaded+=sent;
        return sent;
}




void sendlist(SOCKET s, connection *c) {
		        char tmp[2048];
        addtolog(c->con_id,"200 OK");
        sprintf(tmp,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"DOCTYPE"\r\n<html>\r\n<head>\r\n"LINKCSS"\r\n<title>%s</title>\r\n</head>\r\n<body>\n",server_name);
		sendmsg(s, tmp, c);

        send_file_rangeless(c,motdfilename_f);
        __int64 tsize=0;

        if (showlist) {
        int _error=sendmsg(s,"<p><table class=\"filelist\">\r\n",c);
		int total_visible=0;
		int i;
        for (i=0;(i<shared_files)&&(_error!=SOCKET_ERROR);i++) if (sharedfiles[i].visible) {
	        	total_visible++;
                //char tmp2[2560];
                //geturlforid(i,tmp2);
                __int64 filesize=0;
                char *fn;
                filesize = sharedfiles[i].filesize;

                char tmp3[2560],fn2[2560];
                strcpy(tmp3,sharedfiles[i].filename);
                fn=strtok(tmp3,"\\");
                while(fn!=NULL) {
                        fn=strtok(NULL,"\\");
                        if (fn!=NULL) {strcpy(fn2,fn);}
                }
				char tmp4[80];
				sizechar(tmp4,filesize);
				//gettimestamp(sharedfiles[i].filename,tmp5);
                sprintf(tmp,"<tr><td class=\"filename\"><a href=\"%s\">%s</a></td><td class=\"filedate\">%s</td><td class=\"filesize\">%s</td></tr>\r\n",sharedfiles[i].shared_as,fn2,sharedfiles[i].filedate,tmp4);
                tsize+=filesize;
                _error=sendmsg(s,tmp,c);
        }
        char tmp4[80],tmp[1000];
		sizechar(tmp4,tsize);

        sprintf(tmp,"<tr><td class=\"total\" colspan=\"2\">%s %d %s</td><td class=\"totalsize\">%s</td></tr>\r\n</table>\r\n",Rstr(TEXT_TOTAL),total_visible,Rstr(TEXT_FILES),tmp4);
        sendmsg(s,tmp,c);

        }
        send_sig(s,c);

}

void send_sig(SOCKET s,connection *c)
{
        char tmp[1000];
		sprintf(tmp,"<hr><p class=\"versioninfo\"><a href=\""MINISHARE_URL"\">"version_string"</a> at %s port %d.</p>\r\n</body></html>",server_domain,server_port);
        sendmsg(s,tmp,c);
}







#define MAX_HOSTNAME_SIZE 260

void getownip(char *cip) {
        char HostName[MAX_HOSTNAME_SIZE];
        PHOSTENT pHostEnt;
        IN_ADDR  *ip;

        if (gethostname(HostName, MAX_HOSTNAME_SIZE)==SOCKET_ERROR) {

                return;
        }
        cip[0]='\0';
        ips=0;
        pHostEnt = gethostbyname(HostName);
        while((ip=(struct in_addr *)*pHostEnt->h_addr_list)!=NULL)

                 {

                       strcpy(cipt[ips],inet_ntoa(*ip));
                       ips++;
                       *pHostEnt->h_addr_list++;
                }
         selip=0;
         if ((ips>1)) {
				DialogBox(hInstance,MAKEINTRESOURCE(IP_BOX),0,(DLGPROC) MakeProcInstance((FARPROC)ipproc,hInstance));
				if (selip<0) selip=0;
         }
         strcpy(cip,cipt[selip]);
         if (cip==0) {
	         MessageBox(NULL,"Could not get IP.","Warning",MB_OK);
         }
}



void _cdecl serverthread(void* ptr) {
        server_running=1;
        addtolog(0,"Server started ("VERSION").");
        if (old_engine) addtolog(0,"Using old engine.");
        con_id=0;

        while(1) {
                                        SOCKET active_sock;
                                        SOCKADDR_IN from;
                                        int fromlen = sizeof(from);

                                        active_sock = accept(sock,(SOCKADDR*)&from,&fromlen);
                                        if (active_sock==INVALID_SOCKET) {
                                           break;
                                        }


                                        char tmp[128],addy[513];
                                        HOSTENT *host;
                                        host=gethostbyaddr((char*)&from.sin_addr,4,PF_INET);
                                        if (host!=NULL) {
	                                        sprintf(tmp,"Connect from %s (%s)",host->h_name,inet_ntoa(from.sin_addr));
	                                        strcpy(addy,host->h_name);
	                                        }
	                                    else {
                                        	sprintf(tmp,"Connect from %s",inet_ntoa(from.sin_addr));
                                        	strcpy(addy,inet_ntoa(from.sin_addr));
                                        }

                                        con_id++;
                                        addtolog(con_id,tmp);
                                        connections++;




	                                        if (connections<=max_connections) {
		                                        	int i=0;
		                                        	while((i<256)&&(cons[i].inuse==true)) i++;
		                                        	//cons[i].inuse=true;
		                                        	cons[i].socket=active_sock;
		                                        	strcpy(cons[i].addr,addy);
		                                        	strcpy(cons[i].addr_ip,inet_ntoa(from.sin_addr));
		                                        	cons[i].file=-1;
		                                        	cons[i].request[0]='\0';
		                                        	cons[i].sessionstart=time(NULL);
		                                        	cons[i].last_touch=cons[i].sessionstart;
	                                                cons[i].bytesdownloaded=0;
	                                                cons[i].filesize=-1;
	                                                cons[i].con_id=con_id;


	                                    			if (check_banned(cons[i].addr_ip)||check_banned(cons[i].addr))
                                        			{
	                                    				send_error(active_sock,cons+i,403);
	                                    				connections--;
	                                                	shutdown(active_sock,SD_BOTH);
	                                                	closesocket(active_sock);
	                                                	//cons[i].inuse=false;
                                        			} else {

		                                        		if (old_engine) _beginthread(serve,0,(void*)&(cons[i]));
		                                        		else _beginthread(serve2,0,(void*)&(cons[i]));
	                                        		}

	                                        }  else {
	                                                send_error(active_sock,&(cons[con_id]),503);
	                                                connections--;
	                                                shutdown(active_sock,SD_BOTH);
	                                                closesocket(active_sock);

	                                                }



        }
        addtolog(0,"Server stopped.\n");
        server_running=0;
}

void init_server()
{
	WSADATA wsadata;
    WSAStartup(0x101,&wsadata);
   	getownip(server_domain);

}

void start_server()
{
	   	load_bans(make_workpath_file("banned.txt"));


	 	SOCKADDR_IN local;
		sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.S_un.S_addr = INADDR_ANY;
        local.sin_port = htons( server_port );

        if (bind(sock,(SOCKADDR*) &local,sizeof(local))==SOCKET_ERROR) {
        	MessageBox(NULL,Rstr(TEXT_CANTBIND),Rstr(TEXT_ERROR),0);
        } else {
        	listen(sock,50);
        	int *ptr=NULL;
        	_beginthread(serverthread,0,(void*)ptr);
		}
}

void stop_server()
{
	while (server_running!=0) closesocket(sock);
	int i;
	for(i=0;i<connections;i++) closesocket(cons[i].socket);
}

void deinit_server()
{
	WSACleanup();
}

int kick_user(int id) {
	addtolog(cons[id].con_id,"Kicked user.");
	closesocket(cons[id].socket);
	return 1;
}
