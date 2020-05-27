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
#include "gui.h"
#include "misc.h"
#include "config.h"
#include "files.h"
#include "cmdline.h"
#include "globals.h"
#include <stdio.h>
#include <time.h>
#include <shlobj.h>

extern HIMAGELIST sysimagelist;
extern char text_bytes[32],text_kb[32],text_mb[32],text_gb[32],text_sec[32];

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE phInst,LPSTR cmd,int cmdshow) {

        GetModuleFileName(NULL,minishare_work_path,255);
		int i=strlen(minishare_work_path)-1;
        while (minishare_work_path[i]!='\\')
        	i--;


        minishare_work_path[i]='\0';
        sprintf(filelistfile,"%s\\shared.txt",minishare_work_path);

    	init_server();
		loadini();
		

		InitCommonControls();
		hInstance=hInst;

		char classname[]="MiniShare";
		if (!register_class(hInst,classname)) return 0;

		if (process_command_line(hInst,_argc,_argv)!=0) return 0;
		
		load_users();
        loadurls();

       	strcpy(text_bytes,Rstr(TEXT_BYTES));
		strcpy(text_kb,Rstr(TEXT_KB));
		strcpy(text_mb,Rstr(TEXT_MB));
		strcpy(text_gb,Rstr(TEXT_GB));
		strcpy(text_sec,Rstr(TEXT_SEC));



        if (!init_window(hInst,window_w,window_h,classname))
        {
                WSACleanup();
                return 0;
        }

        srand((unsigned)time(NULL));


        updatelist();

       	start_server();

       	BOOL bRet;
       	MSG msg;

		while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
        {
	    	if (bRet == -1) { } else {

	            TranslateMessage(&msg);
                DispatchMessage(&msg);
                }

        }

		stop_server();
		deinit_server();

        if (logfile!=NULL) fclose(logfile);
		Rstr(-1);
        return msg.wParam ;
}



