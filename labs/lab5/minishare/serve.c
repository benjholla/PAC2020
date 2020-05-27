#include <stdio.h>
#include <winsock2.h>
#include "server.h"
#include "error.h"
#include "minishare.h"
#include "globals.h"

/*

	*** DEPRECATED, SEE serve2.c ***

	OK, this little function needs a total rewrite. It's gets and processes HTTP requests from socket.
	At first, the new function would have to use UNIX style sockets (so you can use sscanf etc. to read
	from the socket), as this one will not get the full header if it's larger than 2000 bytes or is in
	separate packets.

	It works, but really, it's something you wouldn't like to claim yours.

	Basically, we need a new function that works as a thread, gets a connection struct as the parameter,
	reads the request header, processes it and spits the file (or an error message) back to the socket.
	Filesize and modification date should also be updated (in case they have changed). Also, it should
	kill sockets that are idle.	Not a hard task, but since this works, it's been like this for a while.

	*** DEPRECATED ***

*/
void _cdecl serve(void* _cons) {
    SOCKET s=((connection*)_cons)->socket;
    int local_con_id=((connection*)_cons)->con_id;

    char buf[4096],buf2[4096];
	int count=recv(s,buf,4000,0);
	if (count!=SOCKET_ERROR) {

		char bufi[1024];
		int iii=0;
	    buf[count]='\0';
	    strcat(buf,"\r\n\r\n");

		while (buf[iii]>=' ') {
			bufi[iii]=buf[iii];
			iii++;
		}
		bufi[iii]='\0';
		addtolog(local_con_id,bufi);

        strcpy(buf2,buf);

        SendMessage(hWnd,WM_USER+6,0,0);
        SendMessage(hWnd,WM_USER+7,0,0);


        int request_type=0;
        if (strncmp(buf,"GET ",4)==0) request_type=1;
        if (strncmp(buf,"HEAD ",5)==0) request_type=2;
        if ((strlen(buf)>5)&&request_type>0) {

             char req[4096];
                char *tmpr;
                tmpr=strtok(buf," ");
                tmpr=strtok(NULL," ?");
             if (tmpr!=NULL) {
                strcpy(req,tmpr);
                req[4095]='\0';
        if ((strcmp(req,sharepage)==0) &&(infopage)) {
		        strcpy(((connection*)_cons)->request,req);
                sendlist(s,((connection*)_cons));
        } else {


	         	int i=0;
                while ((i<shared_files)&&(strcmp(sharedfiles[i].shared_as,req)!=0)) {
                        i++;
                }
                bool notfound=false;
                char requested_file[512];
                if (i==shared_files) {
	               	if (strcmp(req,"/minishare.css")==0) {	// the css file is shared always
		               	strcpy(requested_file,css_path_f);
	               	} else notfound=true;
                } else {
	             	strcpy(requested_file,sharedfiles[i].filename);
                }

                FILE *f;
                __int64 filesize=0;
                if (!(notfound)) {
                        if ((f = fopen(requested_file, "rb"))!=NULL) {
                        	fseek(f, 0, SEEK_END);
                        	filesize = ftell(f);
                        	sharedfiles[i].filesize=filesize; // Update filesize just in case...
                        	((connection*)_cons)->filesize=filesize;
                        	strcpy(((connection*)_cons)->request,requested_file);
                        	SendMessage(hWnd,WM_USER+7,0,0);
                        	fclose(f);
                        } else {notfound=true;}
                }
                __int64 range_start=0;
                __int64 range_end=filesize;
                char *tmpr;
                tmpr=strtok(buf2,"\n");
                //strcpy(req,tmpr);
                while ((strncmp(req,"Range: ",7)!=0)&&(tmpr!=NULL)&&(strlen(req)>3)) {
                        strcpy(req,strtok(NULL,"\n"));
                }


                if (tmpr!=NULL) {
                        char temp[300];
                        strcpy(temp,&(req[7]));

                        if (strncmp(temp,"bytes=",6)==0) {

                                strcpy(req,strtok(temp,"="));

                                strcpy(req,strtok(NULL,"=-"));
                                range_start=atoi(req);
                                char *tmpr;
                                tmpr=strtok(NULL,"-\r");
                                if (tmpr!=NULL) {
                                        strcpy(req,tmpr);
                                        range_end=atoi(req);
                                        }
                        }
                }

                char tmpfn[5120];
                char temppi[5120],temppi2[5120];


                if (!notfound) {

                	strcpy(tmpfn,requested_file);
                	strcpy(temppi2,getmimetype(tmpfn));
					gettimestamp(tmpfn,temppi);
					strcpy(sharedfiles[i].filedate,temppi);
            	}




                if (notfound) {
                        send_error(s,((connection*)_cons),404);
                } else {
                        char msg[2048];
                        if ((range_start==0)&&(range_end==filesize)) {
                                strcpy(msg,"HTTP/1.1 200 OK\r\n");
                                addtolog(local_con_id,"200 OK");

                        } else {
                                sprintf(msg,"HTTP/1.1 206 Partial Content\r\nAccept-Ranges: bytes\r\nContent-Range: bytes %I64d-%I64d/%I64d\r\n",range_start,range_end,range_end+1);
                                addtolog(local_con_id,"206 Partial Content");

                        }

					    sprintf(buf,"%sLast-Modified: %s\r\nContent-Type: %s\r\nContent-Length: %I64d\r\n\r\n",msg,temppi,temppi2,range_end-range_start);

                        sendmsg(s,buf,((connection*)_cons));

                if (request_type==1) {
                        f = fopen(tmpfn, "rb");
                        strcpy(((connection*)_cons)->request,tmpfn);

                        int _error=0;
                        __int64 totalsent=range_start;
						__int64 lastsent=0;
						((connection*)_cons)->range_start=range_start;
						((connection*)_cons)->range_end=range_end;

                        while ((range_start<filesize)&&(totalsent<range_end)&&(_error!=SOCKET_ERROR)) {

                                if (fseek(f,totalsent,SEEK_SET)==0) {
                                        int length=fread(buf,1,SEND_BUFFER_SIZE,f);
                                        if (totalsent+length>=range_end)  {
                                        	length=range_end-totalsent;
                                        }
                                        _error=send(s,buf,length,0);
                                        totalsent+=_error;
                                        lastsent+=_error;
                                        ((connection*)_cons)->bytesdownloaded+=_error;
                                        if (lastsent>=1024*64) {
	                                        SendMessage(hWnd,WM_USER+7,0,0);
                                        	lastsent=0;
                                        }

                                } else {
                                        break;
                                }


                        }
                        fclose(f);

                  }

                  }
             }

                }


        } else {
                send_error(s,((connection*)_cons),405);
        }
}
        shutdown(s,SD_BOTH);
        closesocket(s);
        connections--;

        char tmp[800];

        sprintf(tmp,"Total %I64d bytes sent.",((connection*)_cons)->bytesdownloaded);

        addtolog(local_con_id,tmp);

        ((connection*)_cons)->inuse=false;

        addtolog(local_con_id,"Connection closed.");

        SendMessage(hWnd,WM_USER+6,0,0);
        SendMessage(hWnd,WM_USER+7,0,0);

}
