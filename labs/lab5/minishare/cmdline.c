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

#include "cmdline.h"
#include "files.h"
#include "minishare.h"
#include "globals.h"
#include "gui.h"
#include "auth.h"

#define NUMPARAMS 9

static const PARAMETER params[NUMPARAMS]=
{
	{CMDLINE_ADD,"add","a",PARAM_STR},
	{CMDLINE_REMOVE,"remove","r",PARAM_STR},
	{CMDLINE_MINIMIZE,"minimize","m",PARAM_NONE},
	{CMDLINE_DIE,"die","d",PARAM_INT},
	{CMDLINE_REMOVEURL,"remove_shared_as","rs",PARAM_STR},
	{CMDLINE_ADDANDCOPY,"add_and_copy","ac",PARAM_STR},
	{CMDLINE_FORCENEWINSTANCE,"force_new_instance","f",PARAM_NONE},
	{CMDLINE_USELIST,"list","l",PARAM_STR},
	{CMDLINE_OLDENGINE,"use_old_engine","",PARAM_NONE}
};

int check_param(int argc,char **argv,int id,void *value,int *st)
{
	int i,pi=0;
	for (i=0;i<NUMPARAMS;i++) if (params[i].id==id) pi=i;

	i=*st;
	if (i<argc)
	{
		/*char tmp[80];
		sprintf(tmp,"%s\n",argv[i]);
		MessageBox(0,tmp,tmp,0);*/
		if (argv[i][0]=='-')
		if (stricmp(argv[i]+1,params[pi].param)==0||stricmp(argv[i]+1,params[pi].param_short)==0)
		{
			if (params[pi].type==PARAM_NONE||argc-i>=2)
			{
				switch (params[pi].type)
				{
					case PARAM_INT:
					{
						*(int*)value=atoi(argv[i+1]);
						*st+=1;
					}
					break;
					case PARAM_STR:
					{
						strcpy((char*)value,argv[i+1]);
						*st+=1;
					}
					break;
					case PARAM_NONE:
					{

					}
					break;
				}
				return 1;
			} else {
				if (params[pi].type==PARAM_INT)
				{
					*(int*)value=0;
					return 1;
				} else {
					strcpy((char*)value,"");
					return 1;
				}
			}
		}
	}
	return 0;
}

int process_command_line(HINSTANCE hInst,int argc,char **argv)
{
		HANDLE hMutex=NULL;
		int returncode=0,st=1,newinstance=0;
		HWND dest=FindWindow("MiniShare",NULL);
    	hMutex=CreateMutex(NULL, TRUE, "MiniShare");	//
    	int dw=GetLastError();							// Check if MiniShare is already running
    	while (st<argc)
    	{

		    if( dw == ERROR_ALREADY_EXISTS ) 				//
	    	{
		    	char parameter[512]="";
		    	int parameter_d=0;
		    	returncode=1;
			    	COPYDATASTRUCT copy;					// ... and pass commands to it
		    		if (check_param(argc,argv,CMDLINE_ADD,parameter,&st))
		    		{
			    		copy.dwData=REMOTE_ADD_FILE;
			    		copy.cbData=512;
			    		copy.lpData=&parameter;
			    		SendMessage(dest,WM_COPYDATA,(WPARAM)(HWND)NULL,(LPARAM)(LPVOID)&copy);
			    		returncode=1;
		    		} else
		    		if (check_param(argc,argv,CMDLINE_ADDANDCOPY,parameter,&st))
		    		{
			    		copy.dwData=REMOTE_ADDANDCOPY_FILE;
			    		copy.cbData=512;
			    		copy.lpData=&parameter;
			    		SendMessage(dest,WM_COPYDATA,(WPARAM)(HWND)NULL,(LPARAM)(LPVOID)&copy);
			    		returncode=1;
		    		} else
		    		if (check_param(argc,argv,CMDLINE_REMOVE,parameter,&st))
		    		{
			    		copy.dwData=REMOTE_REMOVE_FILE;
			    		copy.cbData=512;
			    		copy.lpData=&parameter;
			    		SendMessage(dest,WM_COPYDATA,(WPARAM)(HWND)NULL,(LPARAM)(LPVOID)&copy);
			    		returncode=1;
		    		} else
		    		if (check_param(argc,argv,CMDLINE_REMOVEURL,parameter,&st))
		    		{
			    		copy.dwData=REMOTE_REMOVE_URL;
			    		copy.cbData=512;
			    		copy.lpData=&parameter;
			    		SendMessage(dest,WM_COPYDATA,(WPARAM)(HWND)NULL,(LPARAM)(LPVOID)&copy);
			    		returncode=1;
		    		} else
		    		if (check_param(argc,argv,CMDLINE_DIE,&parameter_d,&st))
	    			{
		    			SendMessage(dest,WM_USER+8,parameter_d*1000,0);
		    			returncode=1;
	    			} else
		    		if (check_param(argc,argv,CMDLINE_FORCENEWINSTANCE,&parameter_d,&st))
		    		{
			    		newinstance=1;
			    		/*SendMessage(dest,WM_USER+5,ID_TRAY,WM_LBUTTONUP); // display previous instance (simulate a keypress)
		    	 		returncode=1;		*/
		    		}


	    	} else {
		    	char parameter[512]="";
		    	int parameter_d=0;

		    		if (check_param(argc,argv,CMDLINE_ADD,parameter,&st))
		    		{
			    		addsharedfile(parameter);
		    		} else
		    		if (check_param(argc,argv,CMDLINE_ADDANDCOPY,parameter,&st))
		    		{
			    		addsharedfile(parameter);
			    		char tmp[512];
			    		geturlforid(shared_files-1,tmp);
			    		copy_to_clipboard(tmp);
		    		} else
			    	if (check_param(argc,argv,CMDLINE_MINIMIZE,NULL,&st))
	    			{
		    			temp_start_minimized=1;
	    			} else
	    			if (check_param(argc,argv,CMDLINE_DIE,&parameter_d,&st))
	    			{
		    			die_in=parameter_d;
	    			} else
					if (check_param(argc,argv,CMDLINE_USELIST,parameter,&st))
	    			{
		    			strcpy(filelistfile,parameter); // bubblegum
		    			loadurls(); // purkka
	    			} else
	    			if (check_param(argc,argv,CMDLINE_OLDENGINE,NULL,&st))
	    			{
		    			old_engine=1;
	    			}
	    	}
	    	st++;
    	}
    	if (argc==1&&dw==ERROR_ALREADY_EXISTS) {
	    	SendMessage(dest,WM_USER+5,ID_TRAY,WM_LBUTTONUP); 	// display previous instance (simulate a keypress)
    		returncode=1;										// if started without parameters
		}

    	if (newinstance) returncode=0;
    	return returncode;
}
