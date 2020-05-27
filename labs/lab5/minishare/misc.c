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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_L_STRING 255 //maximum string length
extern char text_bytes[32],text_kb[32],text_mb[32],text_gb[32],text_sec[32];
char *Rstr(int id)
{
static char *string[8]={NULL};
static int  istring=-1;
int i;

        //-1 will free up memory
 if(id<0){
   for(i=0;i<8;i++){
     if(string[i]!=NULL){
       free(string[i]);
       string[i]=NULL;
     }
   }
          istring=-1; //reset pointer

        }else{

          //if this is the first time, allocate the arrays
   if(string[0]==NULL){
     for(i=0;i<8;i++){
       string[i]=(char*)malloc(MAX_L_STRING+1);
     }
     istring=-1; //reset poinrter
   }

   //now load the string into the next available slot
   istring=(istring+1)%8;
   LoadString(hInstance,id,string[istring],MAX_L_STRING+1);

 } return(string[istring]);
}


void sizechar(char *s,__int64 size) {
	if (size<1024) {
		sprintf(s,"%I64d %s",size,text_bytes);
	}
	if ((size>=1024)) {
		sprintf(s,"%I64d %s",size/1024,text_kb);
	}
	if ((size>=1000*1000)) {
		sprintf(s,"%0.1f %s",(float)size/(1024*1024),text_mb);
	}

	if ((size>=10*1000*1000)) {
		sprintf(s,"%I64d %s",size/(1024*1024),text_mb);
	}

	if ((size>=1000*1000*1000)) {
		sprintf(s,"%0.1f %s",(float)size/(1024*1024*1024),text_gb);
	}

}


void ToLower (char* string)
{
    while (*string) {
       *string = tolower (*string);
       string++;
    }
}

char *make_workpath_file(char *s) {
	static char path[8192];
	if (s[0]!='\\' && s[1]!=':') // check if the path is a relative path
		sprintf(path,"%s\\%s",minishare_work_path,s);
		else return s;
	return path;
}
