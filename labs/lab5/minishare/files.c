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
#include "files.h"
#include "misc.h"
#include "auth.h"
#include <windows.h>
#include <stdio.h>
#include <ctype.h>

SHAREDFILE sharedfiles[MAX_SHARED_FILES+1];

extern char sharepage[128],server_name[256],minishare_work_path[256];
extern int server_port;
char mimetype[MAX_MIMES][64];
char mimetype_ext[MAX_MIMES][8];
char filelistfile[256];
int mimetypes=0;

__int64 get_filesize(char *fn)
{
	FILE *f;
	__int64 filesize=-1;
    if ((f = fopen(fn,"rb"))!=NULL) {
    	fseek(f, 0, SEEK_END);
        filesize=ftell(f);
        fclose(f);
    }
    return filesize;
}


bool GetCreationDate(const char *filename, SYSTEMTIME *st)
{
  HANDLE find_hand;
  WIN32_FIND_DATA wfd;
  FILETIME lfc; // local file time
  bool result = true;

  find_hand = FindFirstFile(filename, &wfd);

  if (find_hand == INVALID_HANDLE_VALUE)
      {
      ZeroMemory(st, sizeof(st));
      result = false;
      }
  else
      {
      FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &lfc);
      FileTimeToSystemTime(&lfc, st);
      }

  return result;
}

char * getmimetype(char *fn) {
        char tmp2[5120];
        strcpy(tmp2,fn);
        static char tmp[256];

        char *f,*ef=NULL;
        f=strtok(tmp2,".");
        while (f!=NULL) {
                ef=f;
                f=strtok(NULL,".");
        }
        char *ext=ef;

        strcpy(tmp,"application/octet-stream");

        if (ext!=NULL)
        {
        	ToLower(ext);
        	int i;
        	for (i=0;(i<mimetypes);i++)
        	{
        		if (strcmp(ext,mimetype_ext[i])==0)
                	strcpy(tmp,mimetype[i]);
        	}
        }
        return tmp;
}

void gettimestamp(char *fn,char *date) {
	SYSTEMTIME create_time;
	static char *daynames[] =
       { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    static char *monthnames[] =
       { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    if (GetCreationDate(fn,&create_time)!=true) {
	    date[0]='\0';
    }

    sprintf(date, "%s, %d %s %d %d:%.2d:%.2d",
        		daynames[create_time.wDayOfWeek],
       			create_time.wDay,
                monthnames[create_time.wMonth-1],
                create_time.wYear,
                create_time.wHour,
                create_time.wMinute,
                create_time.wSecond);

}

void encodeurl(char *url) {
        char *s=url;
        char en[1280*2];
        en[0]=0;
        while ((signed char)*url) {
                if ( (*url<0x20) ||

                     ((unsigned char)*url>=0x80)||

                     (*url==0x24)||
                     (*url==0x26)||
                     (*url==0x27)||
                     (*url==0x2b)||
                     (*url==0x2c)||
                     (*url==0x2f)||
                     (*url==0x3a)||
                     (*url==0x3b)||
                     (*url==0x3d)||
                     (*url==0x3f)||
                     (*url==0x40)||

                     (*url==0x20)||
                     (*url==0x22)||
                     (*url==0x3c)||
                     (*url==0x3e)||
                     (*url==0x23)||
                     (*url==0x25)||
                     (*url==0x7b)||
                     (*url==0x7d)||
                     (*url==0x7c)||
                     (*url==0x5c)||
                     (*url==0x5e)||
                     (*url==0x7e)||
                     (*url==0x5b)||
                     (*url==0x5d)||
                     (*url==0x60) ) {

                        char tmp[strlen(en)+1];
                        strcpy(tmp,en),
                        sprintf(en,"%s%%%02X",tmp,((int)*url)&0xFF);

                     } else {

                        char tmp[4];
                        tmp[0]=*url;
                        tmp[1]=0;
                        strcat(en,tmp);

                     }





                //printf("%s\n",en);
                url++;
        }

        strcpy(s,en);

}




void getsharedurl(char *fn,char *url,int rnd) {
        strcpy(url,"/");
        char tmp;
		int i;
        if (rnd)
        {
	        for (i=0;i<random_letters;i++) {
                itoa(rand() % 36,&tmp,36);
                if (rand()%2==0) tmp=toupper(tmp);
                strcat(url,&tmp);
            }
        	if (random_letters>0) strcat(url,"/");
		}
        char *tok;
        char str[260];
        strcpy(str,fn);
        tok=strtok(str,"\\");

        char ltok[260];
        while (tok!=NULL) {
                strcpy(ltok,tok);
                tok=strtok(NULL,"\\");
        }

        encodeurl(ltok);

        strcat(url,ltok);

}

int recursedir(char *fn, char *baseurl) {
        WIN32_FIND_DATA fd;
        HANDLE h;

        char tmpfn[260];
        sprintf(tmpfn,"%s\\*",fn);

        h=FindFirstFile(tmpfn,&fd);

        int f=(int)h;

        while (f) {

                if ((fd.cFileName[0]!='.')&&(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        char tmpfn[260],tmpurl[1024],tmptmpfn[260];
                        sprintf(tmpfn,"%s\\%s",fn,fd.cFileName);


                        strcpy(tmptmpfn,fd.cFileName);

                        encodeurl(tmptmpfn);

                        sprintf(tmpurl,"%s/%s",baseurl,tmptmpfn);

                        if (recursedir(tmpfn,tmpurl)==0) return 0;



                } else {
                        if ((fd.cFileName[0]!='.')&&(shared_files<MAX_SHARED_FILES)) {


                        char tmpfn[260],tmpurl[260];
						char tmp666[666];
    	                getsharedurl(fd.cFileName,tmp666,0);


                        sprintf(tmpfn,"%s\\%s",fn,fd.cFileName);
                        sprintf(tmpurl,"%s%s",baseurl,tmp666);


                        sharedfiles[shared_files].shared_as=(char*)malloc(strlen(tmpurl)+1);
                        sharedfiles[shared_files].filename=(char*)malloc(strlen(tmpfn)+1);
                        sharedfiles[shared_files].visible=1;
                        sharedfiles[shared_files].filesize=get_filesize(tmpfn);

                        strcpy(sharedfiles[shared_files].shared_as,tmpurl);
                        strcpy(sharedfiles[shared_files].filename,tmpfn);
                        gettimestamp(tmpfn,sharedfiles[shared_files].filedate);

                        shared_files++;

                        } else {if (shared_files>=MAX_SHARED_FILES) return 0;}

                }

                f=(int)FindNextFile(h,&fd);
        }

        return 1;


}

int addsharedfile(char *fn) {

        if (GetFileAttributes(fn)&FILE_ATTRIBUTE_DIRECTORY) {
                char baseurl[260*2];
                getsharedurl(fn,baseurl,1);

                if (recursedir(fn,baseurl)==0) goto exceed;

                        saveurls();

        }
        else {
                exceed:
	        	if (shared_files<MAX_SHARED_FILES) {
                        char url[128*2];
                        getsharedurl(fn,url,1);
                        sharedfiles[shared_files].visible=1;
                        sharedfiles[shared_files].shared_as=(char*)malloc(strlen(url)+1);
                        sharedfiles[shared_files].filename=(char*)malloc(strlen(fn)+1);
                        strcpy(sharedfiles[shared_files].shared_as,url);
                        strcpy(sharedfiles[shared_files].filename,fn);
                        sharedfiles[shared_files].visible=1;
                        sharedfiles[shared_files].auth=-1;
                        sharedfiles[shared_files].filesize=get_filesize(fn);
                        gettimestamp(fn,sharedfiles[shared_files].filedate);


                        shared_files++;
                        saveurls();

                        return 1;
                } else
                { return 0; }

         }
         return 1;
}

void geturlforid(int i,char * url) {
        char sh[1024];
        char auth[MAXLPSIZE*2+1];
        strcpy(auth,"");
        if (i<shared_files) {
	        strcpy(sh,sharedfiles[i].shared_as);
	        if (sharedfiles[i].auth==-1||sharedfiles[i].auth>=tot_users)
	        {


            } else {
	         	sprintf(auth,"%s:%s@",users[sharedfiles[i].auth].login,users[sharedfiles[i].auth].passwd);
            }
        } else { strcpy(sh,sharepage); }




        if (server_port==80) sprintf(url,"http://%s%s%s",auth,server_domain,sh);
                else sprintf(url,"http://%s%s:%d%s",auth,server_domain,server_port,sh);
}

void saveurls() {
        FILE *f;

        f=fopen(filelistfile,"w");

        int i;
        for (i=0;i<shared_files;i++) {
                char tmp[512],login[33];
                if (sharedfiles[i].auth!=-1) strcpy(login,users[sharedfiles[i].auth].login); else login[0]='\0';
                sprintf(tmp,"\"%s\"=%s %d %s\n",sharedfiles[i].filename,sharedfiles[i].shared_as,sharedfiles[i].visible,login);
                fputs(tmp,f);
        }
        fclose(f);
}

void loadurls() {
		shared_files=0;
        FILE *f;

        f=fopen(filelistfile,"r");
        shared_files=0;
        if (f!=NULL) while(!feof(f)){
                char tmp[5130];
                char *k=fgets(tmp,5129,f);

                char tok[5000],val[5000];
                if (k!=NULL)
                {
	                sharedfiles[shared_files].visible=1;
	                char login[50];
	                login[0]='\0';
                        if (sscanf(tmp,"\"%[^\"]\"=%s %d %s",tok,val,&(sharedfiles[shared_files].visible),login)>=2) {
	                        	sharedfiles[shared_files].filename=(char*)malloc(strlen(tok)+1);
                                sharedfiles[shared_files].shared_as=(char*)malloc(strlen(val)+1);
                        		strcpy(sharedfiles[shared_files].filename,tok);
                                strcpy(sharedfiles[shared_files].shared_as,val);
                                if (strlen(login)>0) sharedfiles[shared_files].auth=find_user(login); else
                                sharedfiles[shared_files].auth=-1;
                       			sharedfiles[shared_files].filesize=get_filesize(tok);
                        		gettimestamp(tok,sharedfiles[shared_files].filedate);
                                shared_files++;
                        }
                    }
        }
}

void deletesharedfile(int i,int s) {
        shared_files--;
        free(sharedfiles[i].shared_as);
        free(sharedfiles[i].filename);
        int x;
        for (x=i;x<shared_files;x++)
        {
                sharedfiles[x].filename=sharedfiles[x+1].filename;
                sharedfiles[x].shared_as=sharedfiles[x+1].shared_as;
        }
        if (s!=0)saveurls();
}

void deletesharedfilebyfilename(char*fn) {
        int i;
        for (i=0;i<shared_files;i++)
        {
	     	if (stricmp(sharedfiles[i].filename,fn)==0) deletesharedfile(i,1);
        }
}

void deletesharedfilebyurl(char*url) {
        int i;
        for (i=0;i<shared_files;i++)
        {
	     	if (stricmp(sharedfiles[i].shared_as,url)==0) deletesharedfile(i,1);
        }
}

int findsharedfilebyurl(char *url)
{
	int i;
	for (i=0;i<shared_files;i++)
    {
		if (strcmp(sharedfiles[i].shared_as,url)==0) return i;
	}
	return -1;
}

