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

#include "config.h"
#include "minishare.h"
#include "globals.h"
#include "gui.h"
#include "misc.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

HWND hParam[256];
extern HWND hListbox,hConfigOK,hConfigCancel;

int max_connections=256;
int shared_files=0;
int connections=0;
int infopage=0;
int showlist=1;
int server_running=0,start_minimized=0;
int random_letters=32;
int ini_column[8]={100,150,250,30,200,200,250};
int server_port=80;
int window_h=500,window_w=400;
int config_open=0;
int temp_start_minimized=0;
int die_in=0;
char server_domain[80],con_server_domain[80];
char sharepage[128],server_name[256];
char css_path_f[256],minishare_work_path[256];
char logfilename[2600],logfilename_f[260],motdfilename[2600],motdfilename_f[2600],css_path[2600];

char text_sec[32];
char text_bytes[32],text_kb[32],text_mb[32],text_gb[32];



config_param config_parameters[config_parameter_count]={
{TEXT_SERVERCONF,"",CONFIG_GROUP,NULL},
{TEXT_SERVERPORT,"port",CONFIG_INT,&server_port},
{TEXT_MAXCONS,"maxconnections",CONFIG_INT,&max_connections},

{TEXT_SHAREOPTS,"",CONFIG_GROUP,NULL},
{TEXT_NUMRNDCHARS,"random",CONFIG_INT,&random_letters},
{TEXT_FIXEDDOMAIN,"domain",CONFIG_STR,&con_server_domain},
{TEXT_INDEXPATH,"index",CONFIG_STR,&sharepage},
{TEXT_SHOWLIST,"list",CONFIG_YESNO,&showlist},
{TEXT_INDEXTITLE,"servername",CONFIG_STR,&server_name},

{TEXT_PATHS,"",CONFIG_GROUP,NULL},
{TEXT_MOTD,"motd",CONFIG_STR,&motdfilename},
{TEXT_LOGPATH,"log",CONFIG_STR,&logfilename},
{TEXT_CSSPATH,"css",CONFIG_STR,&css_path},

{TEXT_INTERFACE,"",CONFIG_GROUP,NULL},

{TEXT_STARTMINIMIZED,"minimized",CONFIG_YESNO,&start_minimized},

{0,"",CONFIG_GROUP,NULL}
};




void loadini() {  // this is a mess, fixme


        FILE *f;
        char temp[256];
		sprintf(temp,"%s\\minishare.ini",minishare_work_path);

        strcpy(server_name,"MiniShare");
        f=fopen(temp,"r");
        if (f!=NULL) {
                while (!feof(f)) {
                        char ln[81];
                        fgets(ln,80,f);
                        int i;
                        for (i=0;i<strlen(ln);i++)
                            if (ln[i]<32) ln[i]=0;

                        char *tok,*val;

                        tok=strtok(ln,"=");
                      if (tok!=NULL) {

                        val=strtok(NULL,"=");


                        if (strcmp(tok,"motd")==0) {
                                if (val!=NULL) {
	                                	strcpy(motdfilename,val);
	                                	sprintf(motdfilename_f,"%s\\%s",minishare_work_path,val);

                                        //loadmotd(temp,motd);
                                      	//strcpy(motdfilename,temp);

                                }
                        }

                        if (strcmp(tok,"port")==0) {
                                if (val!=NULL)
                                 server_port=atoi(val);
                                 if (server_port>65535) server_port=65535;
                                 if (server_port<1) server_port=1;
                        }

                        if (strcmp(tok,"index")==0) {
                                if (val!=NULL)
	                        	if (val[0]!='\0') {
	                                if (val[0]!='/') {
		                                sharepage[0]='/';
		                                strcpy(sharepage+1,val);
	                                } else {
                                 		strcpy(sharepage,val); 	}
                                 	infopage=1;

                                 }
                        }

                        if (strcmp(tok,"list")==0) {
                                if ((val!=NULL)&&(strlen(val)>0))
                                      showlist=atoi(val);
                        }


                        if (strcmp(tok,"domain")==0) {
                                if ((val!=NULL)&&(strlen(val)>1)) {
                                 strcpy(con_server_domain,val);
                                 strcpy(server_domain,val);}
                        }

						if (strcmp(tok,"servername")==0) {
                                if ((val!=NULL)&&(strlen(val)>1)) {
                                 strcpy(server_name,val);}
                        }


                        if (strcmp(tok,"maxconnections")==0) {
                                if ((val!=NULL)&&(strlen(val)>0))
                                 max_connections=atoi(val);
                                if (max_connections>256) max_connections=256;
                                //printf("%d\n",max_connections);
                        }

                        if (strcmp(tok,"random")==0) {
                                if ((val!=NULL)&&(strlen(val)>0))
                                 random_letters=atoi(val);
                                //printf("%d\n",max_connections);
                        }


                        if (strcmp(tok,"log")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
	                                char temp[256];
	                                sprintf(temp,"%s\\%s",minishare_work_path,val);

                                    logfile=fopen(temp,"at");
                                    strcpy(logfilename,val);
                                    sprintf(logfilename_f,"%s\\%s",minishare_work_path,val);
                               	}
                        }

                        if (strcmp(tok,"css")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      strcpy(css_path,val);
                                      sprintf(css_path_f,"%s\\%s",minishare_work_path,val);
                                      }
                        }



                        if (strcmp(tok,"minimized")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      start_minimized=atoi(val);
                                      }
                        }


                        if (strcmp(tok,"windoww")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      window_w=atoi(val);
                                      }
                        }
						if (strcmp(tok,"windowh")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      window_h=atoi(val);
                                      }
                        }


                        if (strcmp(tok,"column0")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[0]=atoi(val);
                                      }
                        }

						if (strcmp(tok,"column1")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[1]=atoi(val);
                                      }
                        }
						if (strcmp(tok,"column2")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[2]=atoi(val);
                                      }
                        }

                        if (strcmp(tok,"column3")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[3]=atoi(val);
                                      }
                        }

						if (strcmp(tok,"column4")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[4]=atoi(val);
                                      }
                        }
						if (strcmp(tok,"column5")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[5]=atoi(val);
                                      }
                        }
                        if (strcmp(tok,"column6")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[6]=atoi(val);
                                      }
                        }
                        if (strcmp(tok,"column7")==0) {
                                if ((val!=NULL)&&(strlen(val)>0)) {
                                      ini_column[7]=atoi(val);
                                      }
                        }


                      }


                }
                fclose(f);
        }

        sprintf(temp,"%s\\mimetypes.ini",minishare_work_path);
        f=fopen(temp,"r");
        if (f!=NULL) {
                char ln[81];
                int i=0;

                while(!feof(f)) {
                fgets(ln,80,f);

                sscanf(ln,"%[^=]=%s\n",mimetype_ext[i],mimetype[i]);
                i++;
                }
                mimetypes=i;
                fclose(f);

        }

}

void save_config() {
	int i;
	char temp[256];
	sprintf(temp,"%s\\minishare.ini",minishare_work_path);
	FILE *f=fopen(temp,"w");
	for (i=0;i<config_parameter_count;i++) {
		char tmp[80];
		if (config_parameters[i].type!=CONFIG_GROUP) {
		if (config_parameters[i].type==CONFIG_STR) { sprintf(tmp,"%s=%s\n",config_parameters[i].infile,(char*)config_parameters[i].ptr);}
			else { sprintf(tmp,"%s=%d\n",config_parameters[i].infile,*((int*)config_parameters[i].ptr)); }

		fputs(tmp,f);
		}

	}
	for (i=0;i<3;i++) {
		LVCOLUMN lvc;
		lvc.mask=LVCF_WIDTH;
		SendMessage(hListbox,LVM_GETCOLUMN,i,(WPARAM)&lvc);
		ini_column[i]=lvc.cx;
		fprintf(f,"column%d=%d\n",i,lvc.cx);
	}
	for (i=0;i<4;i++) {
		LVCOLUMN lvc;
		lvc.mask=LVCF_WIDTH;
		SendMessage(hHostview,LVM_GETCOLUMN,i,(WPARAM)&lvc);
		ini_column[i+3]=lvc.cx;
		fprintf(f,"column%d=%d\n",i+3,lvc.cx);
	}
	fprintf(f,"windoww=%d\nwindowh=%d\n",window_w,window_h);
	fclose(f);
}

void config_diag() {
			if (config_open) return;
			config_open=1;
	        HWND hWnd=CreateWindowEx(0,"MiniShareConfig",Rstr(TEXT_MINISHARECONFIG),(WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX))|DS_CENTER ,
                         0,0,200+160+8+8+30-2+30+32,config_parameter_count*19+50-18+GetSystemMetrics(SM_CYCAPTION),
                         0,NULL,hInstance,NULL);
			centerwindow(hWnd);
			ShowWindow(hWnd,SW_NORMAL);
    		HFONT hFont = CreateFont(8 *(-1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,(LPCTSTR) "MS Sans Serif");

    		int i,yo=0,ty=16;

    		HWND gro=NULL;

    		for (i=0;i<config_parameter_count;i++) {
    			if (config_parameters[i].type==CONFIG_GROUP) {
	    			if (gro!=NULL) {
		    			yo+=2;

		    			MoveWindow(gro,0+8,ty-16,367+30+30+16,i*19+yo-ty+16,TRUE);
		    			ty=i*(19)+yo+16;
	    			}
	    			if (i<config_parameter_count-1) {
	    				gro=CreateWindowEx(0,"BUTTON",Rstr(config_parameters[i].desc),
    							WS_CHILD|WS_VISIBLE|BS_GROUPBOX,0,i*19+yo,260,14,
								hWnd,(HMENU)0,hInstance,NULL);
						SendMessage(gro,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);


					}

				} else {

	    		HWND tmp=CreateWindowEx(0,"STATIC",Rstr(config_parameters[i].desc),
					WS_CHILD|WS_VISIBLE,5+16,i*19+yo-2,260,14,
					hWnd,(HMENU)0,hInstance,NULL);
        		SendMessage(tmp,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);
        		if (config_parameters[i].type!=CONFIG_YESNO) {
        		int w;
        		char d[2600];
        		if (config_parameters[i].type==CONFIG_STR) { w=8*20;
        			strcpy(d,(char*)config_parameters[i].ptr);
        		} else {
	        		w=8*6;
	        		sprintf(d,"%d",*((int*)config_parameters[i].ptr));
        		}



				hParam[i]=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",d,
					WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP,260+16,i*19+yo-4,w+2,19,
					hWnd,(HMENU)(6700+i),hInstance,NULL);
        		SendMessage(hParam[i],WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);

        		} else {
	        		hParam[i]=CreateWindowEx(0,"BUTTON","Checkbox",
						WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP,260+1+16,i*19+yo-2,14,14,
						hWnd,(HMENU)(6700+i),hInstance,NULL);
					if (*((int*)config_parameters[i].ptr)!=0) {
						SendMessage(hParam[i],BM_SETCHECK,1,1);
					}



        		}
			}
				}
			hConfigOK=CreateWindowEx(WS_EX_CONTROLPARENT,"BUTTON",Rstr(TEXT_OK),BS_DEFPUSHBUTTON|WS_TABSTOP|WS_CHILD|WS_VISIBLE,0+189+30+16,config_parameter_count*19+8-16+6,68,20,
					hWnd,(HMENU)IDCONFIGOK,hInstance,NULL);
			SendMessage(hConfigOK,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);

			hConfigCancel=CreateWindowEx(WS_EX_CONTROLPARENT,"BUTTON",Rstr(TEXT_CANCEL),BS_PUSHBUTTON|WS_TABSTOP|WS_CHILD|WS_VISIBLE,70+189+30+16,config_parameter_count*19+8-16+6,68,20,
					hWnd,(HMENU)IDCONFIGCANCEL,hInstance,NULL);
			SendMessage(hConfigCancel,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);

			hConfigCancel=CreateWindowEx(WS_EX_CONTROLPARENT,"BUTTON",Rstr(TEXT_APPLY),BS_PUSHBUTTON|WS_TABSTOP|WS_CHILD|WS_VISIBLE,140+189+30+16,config_parameter_count*19+8-16+6,68,20,
					hWnd,(HMENU)IDCONFIGAPPLY,hInstance,NULL);
			SendMessage(hConfigCancel,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);

			HWND tmp=CreateWindowEx(0,"STATIC",Rstr(TEXT_RESTARTSERVER),WS_DISABLED|WS_CHILD|WS_VISIBLE,13,config_parameter_count*19+8-16,188,20+12,
					hWnd,(HMENU)IDCONFIGAPPLY,hInstance,NULL);
			SendMessage(tmp,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);
}

void run_config(HWND h) {
	int i;
	for (i=0;i<config_parameter_count;	i++) {
		char tmp[81];
		GetDlgItemText(h,6700+i,tmp,80);

		switch (config_parameters[i].type) {
			case CONFIG_YESNO: {
				*(int*)config_parameters[i].ptr=SendMessage(hParam[i],BM_GETCHECK,0,0);


				break;
			}
			case CONFIG_STR: {
				strcpy((char*)config_parameters[i].ptr,tmp);
				break;
			}
			case CONFIG_INT: {
				*(int*)config_parameters[i].ptr=atoi(tmp);

				break;
			}
		}
	}

}

LRESULT CALLBACK configproc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (LOWORD(message)) {
				case WM_DESTROY:
					config_open=0;
				break;
                case WM_CREATE:
         		break;

         		case WM_COMMAND: {
	         		switch (HIWORD(wParam)) {
		         		case BN_CLICKED:
		         			switch (LOWORD(wParam)) {
		         				case (IDCONFIGAPPLY):
			         			case (IDCONFIGOK): {
			         				run_config(hwnd);
			         				save_config();
			         				stop_server();
			         				loadini();
			         				start_server();

				         			if (LOWORD(wParam)==IDCONFIGAPPLY) break;
			         			}
		         				case (IDCONFIGCANCEL): {
									SendMessage(hwnd,WM_CLOSE,0,0);
								break;
		         				}
	         				}

		         		break;



	         		}
	         		break;
	         		}


        }
        return DefWindowProc(hwnd, message, wParam, lParam);;

}






