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

#include "gui.h"
#include "misc.h"
#include "minishare.h"
#include "globals.h"
#include "files.h"
#include "config.h"
#include "ban.h"
#include "log.h"
#include "auth.h"
#include "editfile.h"
#include "edituser.h"

#include <shlobj.h>
#include <shellapi.h>
#include <time.h>
#include <math.h>


int minimized=0,icon_send=0,icon_state[2];

HWND hListbox,hConfigOK,hConfigCancel,hDrag,hHostview,hStatusBar;
HMENU hPopup,hAuthmenu;
HIMAGELIST FilesImageList,sysimagelist;

NOTIFYICONDATA note;
HWND hWnd;
HINSTANCE hInstance;

int statusbarheight;

extern connection cons[256];

void centerwindow(HWND hwnd)
{
	RECT rect,drect;
	GetWindowRect(hwnd,&rect);
	GetWindowRect(GetDesktopWindow(),&drect);
	MoveWindow(hwnd,((drect.right)/2-(rect.right-rect.left)/2),((drect.bottom)/2-(rect.bottom-rect.top)/2),(rect.right-rect.left),(rect.bottom-rect.top),TRUE);
}

void getdir(char *path,HWND h)
{
    BROWSEINFO bi = { 0 };
    bi.hwndOwner=h;
    bi.lpszTitle = Rstr(TEXT_SELECTDIR);
    char tmp[260]="";
    bi.pszDisplayName = tmp;
    bi.ulFlags=BIF_USENEWUI|BIF_RETURNONLYFSDIRS|BIF_VALIDATE;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        if (!SHGetPathFromIDList(pidl,(char*) path)) { path[0]='\0'; return; };
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl ); // g++ is used just because of these two lines. stupid.
            imalloc->Release ( );   // FIXME
        }
    } else { path[0]='\0'; }
}

void updatehostview()
{
		SCROLLINFO s;
        s.cbSize=sizeof(SCROLLINFO);
        s.fMask=SIF_POS;

        #define OBJID_VSCROLL 0xFFFFFFFB
		#define OBJID_HSCROLL 0xFFFFFFFA

        GetScrollInfo(hHostview,OBJID_HSCROLL,&s);
        int sx=s.nPos;
		GetScrollInfo(hHostview,OBJID_VSCROLL,&s);
        int sy=s.nPos;

	    SendMessage(hHostview,LVM_DELETEALLITEMS,0,0);
	    int ii=0;
        for (int i=0;i<256;i++) {

        	if ((cons[i]).inuse) {
	        	connection c;
	        	cons[i].listpos=ii;
	        	memcpy(&c,(void*)&(cons[i]),sizeof(connection));
                LV_ITEM lvitem;
				lvitem.mask=LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;
				lvitem.state=lvitem.stateMask=0;
				lvitem.iItem=ii;
				lvitem.iSubItem=0;

				char tmp[260];
				sprintf(tmp,"%d",c.con_id);
				lvitem.pszText=tmp;
				lvitem.cchTextMax=260;
				lvitem.iImage=icon_send;
				lvitem.lParam = ii++;
				SendMessage(hHostview,LVM_INSERTITEM,0,(WPARAM)&lvitem);
				lvitem.iSubItem=1;
				lvitem.pszText=c.addr;
				lvitem.mask=LVIF_TEXT;
				SendMessage(hHostview,LVM_SETITEM,0,(WPARAM)&lvitem);
				lvitem.iSubItem=2;
				lvitem.pszText=c.request;
				lvitem.mask=LVIF_TEXT;
				SendMessage(hHostview,LVM_SETITEM,0,(WPARAM)&lvitem);
				if (c.filesize!=-1) {
					char tmp2[32],speedtmp[64];
					sizechar(tmp2,c.bytesdownloaded);
					sizechar(speedtmp,(__int64)trunc((float)c.bytesdownloaded/(((float)time(NULL)-(float)c.sessionstart)+0.001)));
					strcat(speedtmp,text_sec);
					sprintf(tmp,"%.1f%% (%s, %s)",((float)c.bytesdownloaded)/((float)c.range_end-(float)c.range_start)*100,tmp2,speedtmp);
				} else {
					tmp[0]='\0';
				}

				lvitem.iSubItem=3;

				lvitem.pszText=tmp;
				lvitem.mask=LVIF_TEXT;
				SendMessage(hHostview,LVM_SETITEM,0,(WPARAM)&lvitem);
			}
        }

        s.nPos=sx;
        SetScrollInfo(hHostview,SB_HORZ,&s,false);
       	s.nPos=sy;
        SetScrollInfo(hHostview,SB_VERT,&s,true);
}

void updatelist()
{
        SendMessage(hListbox,LVM_DELETEALLITEMS,0,0);
        int i;
        for (i=0;i<shared_files;i++) {
	        	char tmp[300];
				strcpy(tmp,sharedfiles[i].filename);
				/*SHFILEINFO SFI;
				int flags;
				flags=SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES;
				SHGetFileInfo(sharedfiles[i].filename, 0, &SFI, sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
   	            */
                LV_ITEM lvitem;

				lvitem.state=lvitem.stateMask=0;
				lvitem.iItem=i;
				lvitem.iSubItem=0;

				char tmp2[5120];
        		strcpy(tmp2,sharedfiles[i].filename);

        		char *f,*ef=NULL;
        		f=strtok(tmp2,"\\");
        		while (f!=NULL)
        		{
                	ef=f;
	                f=strtok(NULL,"\\");
    		    }

				lvitem.pszText=ef;
				lvitem.cchTextMax=260;
				lvitem.iImage=icon_state[sharedfiles[i].visible];

				lvitem.mask=LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE;
				lvitem.lParam = i;
				SendMessage(hListbox,LVM_INSERTITEM,0,(WPARAM)&lvitem);

				lvitem.iSubItem=1;
				lvitem.pszText=sharedfiles[i].filename;
				lvitem.mask=LVIF_TEXT;
				SendMessage(hListbox,LVM_SETITEM,0,(WPARAM)&lvitem);
				lvitem.iSubItem=2;
				lvitem.pszText=sharedfiles[i].shared_as;
				lvitem.mask=LVIF_TEXT;
				SendMessage(hListbox,LVM_SETITEM,0,(WPARAM)&lvitem);
        }


        if (shared_files==0)
        {
	     	ShowWindow(hDrag,SW_SHOW);
        }
        else
        {
	     	ShowWindow(hDrag,SW_HIDE);
        }
}

void deletefiles()
{
    int totitems=shared_files;
    if (totitems>0)  {
	    int buf[MAX_SHARED_FILES];

	    int i;
	    int bi=0;
	    for (i=0;i<totitems;i++) {
	    	if (SendMessage(hListbox,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0)
	    	{
	    		buf[bi]=i;
	    		bi++;
	    	}
	    }
	    int d=0;
	    for (i=0;i<bi;i++) {
	    	deletesharedfile(buf[i]-d,(i+1)<bi?0:1);
	        d++;
	    }
	    updatelist();
    }
}

void copy_to_clipboard(char *data)
{
	OpenClipboard(NULL);
    EmptyClipboard();
    HGLOBAL clp=GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,strlen(data)+1);
    LPTSTR clp2;
    clp2=(LPTSTR)GlobalLock(clp);
    memcpy(clp2,data,strlen(data));
    clp2[strlen(data)]=(TCHAR)0;
	GlobalUnlock(clp);
    SetClipboardData(CF_TEXT,clp);
    CloseClipboard();
}

void build_user_menu(HMENU h,int auth) {
	int f=MF_STRING;
	if (auth==-1) f|=MF_CHECKED;
	AppendMenu(h,f,IDM_AUTH_NONE,Rstr(TEXT_NONE));
	AppendMenu(h,MF_SEPARATOR,0,NULL);

	if (tot_users>0) {
		int i;
		for (i=0;i<tot_users;i++) {
			f=MF_STRING|((i==auth)?MF_CHECKED:MF_UNCHECKED);
			AppendMenu(h,f,IDM_AUTH_NONE+1+i,users[i].login);
		}
		AppendMenu(h,MF_SEPARATOR,0,NULL);

	}
	AppendMenu(h,MF_STRING,IDM_ADDLOGIN,Rstr(TEXT_ADDLOGIN));


}

LRESULT CALLBACK wndproc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        char caption[78];
        switch (LOWORD(message)) {

				case WM_CREATE:

                note.cbSize=sizeof(note);
                note.uID=ID_TRAY;
                note.hWnd=hwnd;
                note.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
                note.uCallbackMessage=WM_USER+5;
                if (connections>0) {
                	note.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(MINISHARE_ICON_BUSY));
                } else {
                	note.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(MINISHARE_ICON));

                }

                strcpy(note.szTip,"MiniShare");

                Shell_NotifyIcon(NIM_ADD,&note);

				if (start_minimized||temp_start_minimized)
        		{

	        		SendMessage(hwnd,WM_SIZE,SIZE_MINIMIZED,0);
	        		//ShowWindow(hwnd,SW_HIDE);
    			}
    			if (die_in>0)
    			{
	    			SetTimer(hwnd,WM_DESTROY,die_in*1000,NULL);
    			}

                return 0;
                break;
                case WM_COMMAND:
                		if (LOWORD(wParam)>=IDM_AUTH_NONE) {
	                		//char tmp[80];
	                		//sprintf(tmp,"%d",LOWORD(wParam));

	                		int i;
	                		for (i=0;i<shared_files;i++) {
                            	if (SendMessage(hListbox,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0){
	                            	sharedfiles[i].auth=LOWORD(wParam)-(IDM_AUTH_NONE+1);
                                }
                         	}
                         	saveurls();
                		} else
                        switch (LOWORD(wParam)) {
	                        	case IDM_ADDFILE:
	                        	{
		                        	OPENFILENAME ofn;
		                        	memset(&ofn,0,sizeof(OPENFILENAME));
		                        	ofn.lStructSize = sizeof(OPENFILENAME);
									char szFile[5000];
									szFile[0]='\0';
		                        	ofn.lpstrFile = szFile;
									ofn.hwndOwner = hwnd;
		                        	ofn.nMaxFile = sizeof(szFile);
		                        	ofn.nFilterIndex = 1;
									ofn.lpstrFileTitle = NULL;
									ofn.nMaxFileTitle = 0;
									ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY|OFN_EXPLORER;
									ofn.lpstrFilter = "All Files\0*.*\0\0";
									ofn.nFilterIndex = 1;
		                        	if (GetOpenFileName(&ofn)) {
			                        	char *b=szFile,*files[5000];
			                        	int i=0,ii;
			                        	while (*b!='\0') {
				                        	files[i]=b;
				                        	b+=strlen(b)+1;
				                        	i++;
			                        	}
			                        	if (i<=1) {
				                			if (addsharedfile(files[0])==0) {
	                							MessageBox(hwnd,Rstr(TEXT_MAXSHAREDEXCEEDED) ,Rstr(TEXT_WARNING),MB_OK);

                                			}
                                		} else {
				                        	for (ii=1;ii<i;ii++) {
					                        	char tmp[500];
					                        	sprintf(tmp,"%s\\%s",files[0],files[ii]);
					                        	if (addsharedfile(tmp)==0) {
	                								MessageBox(hwnd,Rstr(TEXT_MAXSHAREDEXCEEDED) ,Rstr(TEXT_WARNING),MB_OK);
	                                				break;
                                					}
                                			}
			                        }
			                        updatelist();
		                        	}
	                        	}
	                        	break;
	                        	case IDM_ADDDIR:
	                        	{
		                        	char tmp[500];
		                        	getdir(tmp,hwnd);
		                        	if (tmp[0]!='\0') if (addsharedfile(tmp)==0) {
	                						MessageBox(hwnd,Rstr(TEXT_MAXSHAREDEXCEEDED) ,Rstr(TEXT_WARNING),MB_OK);

                                			}

		                        	updatelist();
	                        	}
	                        	break;
                                case IDM_ABOUT:
                                {
	                                	DialogBox(hInstance,MAKEINTRESOURCE(ABOUT_BOX),hwnd,(DLGPROC) MakeProcInstance((FARPROC)aboutproc,hInstance));
	                                	break;
                                }
                                break;
                                case IDM_LOG:
                                {
                                        char tmp[256];
                                        sprintf(tmp,"notepad.exe %s",logfilename_f);
                                        WinExec(tmp,1);
                                        break;
                                }
                                break;
                                case IDM_REMOVEFILE:
                                {
                                        deletefiles();
                                        break;
                                }
                                break;
                                case IDM_CONFIG:
                                {
	                                	config_diag();
	                                	break;
	                            }
	                            case IDM_SHOWHIDE:
	                            	int i;
	                            	for (i=0;i<shared_files;i++) {
                                        	if (SendMessage(hListbox,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0){
                                        		sharedfiles[i].visible=!sharedfiles[i].visible;
                                        		}
                                        }
                                    updatelist();
                                    saveurls();
	                            break;
								case IDM_SHOWFILES:
								    ShowWindow(hHostview,SW_HIDE);
        							ShowWindow(hListbox,SW_SHOW);
								break;
								case IDM_SHOWCONNECTIONS:
									ShowWindow(hHostview,SW_SHOW);
        							ShowWindow(hListbox,SW_HIDE);
								break;
                                case IDM_COPYINDEX:
                                case IDM_COPYURL:
                                {

                                    char url[512*130*2];
                                    int totitems;
                                    if (LOWORD(wParam)==IDM_COPYINDEX) {
                                        geturlforid(65536,url);
                                        totitems=1;

                                    } else {
                                        totitems=shared_files;

	                                    if (totitems==0) break;

                                        int buf[MAX_SHARED_FILES];

                                        int i;
                                        int bi=0;
                                        for (i=0;i<totitems;i++) {
                                        	if (SendMessage(hListbox,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0){
                                        		buf[bi]=i;
                                        		bi++;
                                        		}
                                        }





                                        *url=0;
                                        for (int i=0;i<bi;i++) {
                                                char tmp[128*2];
                                                geturlforid(buf[i],tmp);
                                                strcat(url,tmp);

                                                if (bi>1) strcat(url,"\r\n");
                                        }
                                     }

                                     copy_to_clipboard(url);
                                        //GlobalFree(buf);
                                }
                                break;

                                case IDM_KICKBAN:
                                case IDM_KICK:
                                {
	                                int i;
	                                for (i=0;i<connections;i++) {
                                      	if (SendMessage(hHostview,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0){

	                                      	int c;

	                                      	for (c=0;c<256&&cons[c].listpos!=i;c++);

	                                      	if (LOWORD(wParam)==IDM_KICKBAN) {
		                                      	add_ban(cons[c].addr_ip,-1);
		                                      	addtolog(cons[c].con_id,"Banned user.");
	                                      	} else {
		                                      	add_ban(cons[c].addr_ip,30);
	                                      	}

                                       		kick_user(c);
                                    	}
                                    }
                                    SendMessage(hWnd,WM_USER+6,0,0);
    								SendMessage(hWnd,WM_USER+7,0,0);
                                }
                                break;
                                
                                case IDM_EDITFILE: {
									int i,c=-1;
	                            	for (i=0;(i<shared_files)&&(c==-1);i++) if (SendMessage(hListbox,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0) c=i;
	                            	edit_file=c;
	                            	
	                           		DialogBox(hInstance,MAKEINTRESOURCE(EDIT_FILE_BOX),hwnd,(DLGPROC) MakeProcInstance((FARPROC)editfileproc,hInstance));
	                           		updatelist();
									saveurls();
	                                
                                } break;
                                case IDM_ADDLOGIN: {
	                                DialogBox(hInstance,MAKEINTRESOURCE(EDIT_USER_BOX),hwnd,(DLGPROC)MakeProcInstance((FARPROC)edituserproc,hInstance));
	                           		saveurls();
                                } break;
                        }
                break;

                case WM_DROPFILES:
                {
                        int totfiles=DragQueryFile((HDROP)wParam,0xffffffff,NULL,0);

                        for (int i=0;i<totfiles;i++) {
                                char fn[260];
                                DragQueryFile((HDROP)wParam,i,fn,260);
                                if (addsharedfile(fn)==0) {
	                				MessageBox(hwnd,Rstr(TEXT_MAXSHAREDEXCEEDED) ,Rstr(TEXT_WARNING),MB_OK);
	                                break;
                                }
                        }
                        updatelist();
                }
                break;
				/*case WM_LBUTTONDBLCLK : {
                //        if ((HWND)wParam==hListbox) {
	                        SendMessage(hWnd,WM_COMMAND,IDM_EDITFILE,0);
                //       }
            	} break;*/
                case WM_CONTEXTMENU: {

                        if ((HWND)wParam==hListbox) {
	                        if (hPopup!=NULL) DestroyMenu(hPopup);
	                        hPopup=CreatePopupMenu();

	                        AppendMenu(hPopup,MF_STRING,IDM_ADDFILE,Rstr(TEXT_ADDFILES));
                        	SetMenuItemBitmaps(hPopup,IDM_ADDFILE,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_OPEN)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_OPEN)));
                        	AppendMenu(hPopup,MF_STRING,IDM_ADDDIR,Rstr(TEXT_ADDDIR));
                        	SetMenuItemBitmaps(hPopup,IDM_ADDDIR,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_DIR)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_DIR)));

                        int selected;
	                    if ((selected=SendMessage(hListbox,LVM_GETSELECTEDCOUNT,0,0))>0) {
		                    	AppendMenu(hPopup,MF_SEPARATOR,0,NULL);
                        		AppendMenu(hPopup,MF_STRING,IDM_COPYURL,Rstr(TEXT_COPYURL));
                        		AppendMenu(hPopup,MF_STRING,IDM_REMOVEFILE,Rstr(TEXT_REMOVE));
                        		AppendMenu(hPopup,MF_STRING,IDM_SHOWHIDE,Rstr(TEXT_SHOWHIDE));

                        			if (hAuthmenu!=NULL) DestroyMenu(hAuthmenu);
	                        		hAuthmenu=CreatePopupMenu();
	                        		int u;
	                        		if (selected==1) {
		                        		for (int i=0;(i<shared_files);i++) if (SendMessage(hListbox,LVM_GETITEMSTATE,(WPARAM)i,(LPARAM)LVIS_SELECTED)!=0) u=i;
		                        		/*char tmp[80];
		                        		sprintf(tmp,"%d",u);
		                        		MessageBox(NULL,tmp,tmp,MB_OK);*/
		                        		u=sharedfiles[u].auth;
	                        		} else u=-2;
	                        		build_user_menu(hAuthmenu,u);
	                        		AppendMenu(hPopup,MF_POPUP,(UINT)hAuthmenu,Rstr(TEXT_AUTHMENU));

	                        	SetMenuItemBitmaps(hPopup,IDM_COPYURL,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_COPY)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_COPY)));
                        		SetMenuItemBitmaps(hPopup,IDM_REMOVEFILE,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_REMOVE)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_REMOVE)));
                        		SetMenuItemBitmaps(hPopup,IDM_SHOWHIDE,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_SHOWHIDE)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_SHOWHIDE)));

                        }
                        if (sharepage[0]!=0) {
	                        AppendMenu(hPopup,MF_SEPARATOR,0,NULL);
                            AppendMenu(hPopup,MF_STRING,IDM_COPYINDEX,Rstr(TEXT_COPYINDEX));

                        	SetMenuItemBitmaps(hPopup,IDM_COPYINDEX,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_INDEX)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_INDEX)));
                        }
                        AppendMenu(hPopup,MF_SEPARATOR,0,NULL);


                        if (logfile!=NULL) {
	                       	AppendMenu(hPopup,MF_STRING,IDM_LOG,Rstr(TEXT_OPENLOG));
                        	SetMenuItemBitmaps(hPopup,IDM_LOG,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_LOG)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_LOG)));
                        }



                        AppendMenu(hPopup,MF_STRING,IDM_CONFIG,Rstr(TEXT_CONFIG));
                        AppendMenu(hPopup,MF_STRING,IDM_ABOUT,Rstr(TEXT_ABOUT));
                        SetMenuItemBitmaps(hPopup,IDM_ABOUT,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_ABOUT)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_ABOUT)));
                        SetMenuItemBitmaps(hPopup,IDM_CONFIG,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_CONFIG)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_CONFIG)));

                        AppendMenu(hPopup,MF_SEPARATOR,0,NULL);

                        AppendMenu(hPopup,MF_STRING | MF_CHECKED,IDM_SHOWFILES,Rstr(TEXT_SHOWFILES));
                        AppendMenu(hPopup,MF_STRING,IDM_SHOWCONNECTIONS,Rstr(TEXT_SHOWCONNECTIONS));
                        
                        if (selected) {
							AppendMenu(hPopup,MF_SEPARATOR,0,NULL);
                        	AppendMenu(hPopup,MF_STRING,IDM_EDITFILE,Rstr(TEXT_EDITFILE));
                    	}

                                POINT pt;
                                GetCursorPos(&pt);
                                TrackPopupMenu(hPopup,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);

                  }
                  	if ((HWND)wParam==hHostview) {
	                	if (hPopup!=NULL) DestroyMenu(hPopup);
	                    hPopup=CreatePopupMenu();


	                    AppendMenu(hPopup,MF_STRING,IDM_KICK,Rstr(TEXT_KICK));
	                    AppendMenu(hPopup,MF_STRING,IDM_KICKBAN,Rstr(TEXT_KICKBAN));
	                    AppendMenu(hPopup,MF_SEPARATOR,0,NULL);

                        if (logfile!=NULL) {
	                        	 AppendMenu(hPopup,MF_STRING,IDM_LOG,Rstr(TEXT_OPENLOG));
                        		SetMenuItemBitmaps(hPopup,IDM_LOG,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_LOG)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_LOG)));
                                  }


                        AppendMenu(hPopup,MF_STRING,IDM_CONFIG,Rstr(TEXT_CONFIG));
                        AppendMenu(hPopup,MF_STRING,IDM_ABOUT,Rstr(TEXT_ABOUT));
                        SetMenuItemBitmaps(hPopup,IDM_ABOUT,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_ABOUT)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_ABOUT)));
                        SetMenuItemBitmaps(hPopup,IDM_CONFIG,MF_BYCOMMAND,LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_CONFIG)),LoadBitmap(hInstance,MAKEINTRESOURCE(MENU_CONFIG)));


                        AppendMenu(hPopup,MF_SEPARATOR,0,NULL);

                        AppendMenu(hPopup,MF_STRING,IDM_SHOWFILES,Rstr(TEXT_SHOWFILES));
                        AppendMenu(hPopup,MF_STRING | MF_CHECKED,IDM_SHOWCONNECTIONS,Rstr(TEXT_SHOWCONNECTIONS));



	                  		POINT pt;
                          	GetCursorPos(&pt);
                          	TrackPopupMenu(hPopup,TPM_LEFTALIGN,pt.x,pt.y,0,hwnd,NULL);
                  	}
	              }
                break;
                case WM_CTLCOLORSTATIC: {

	                if (((HWND)lParam) !=  hDrag)
	  					break;
      				return (BOOL)GetStockObject (NULL_BRUSH);

      			}

      		    case WM_GETMINMAXINFO: {
	      		    MINMAXINFO *Struct1;
  					Struct1 = (MINMAXINFO *)lParam;
  					Struct1->ptMinTrackSize.x=300;
  					Struct1->ptMinTrackSize.y=68+statusbarheight;
  					return 0;

	      		    break;
      		    }

                case WM_SIZE:
                        switch (wParam) {
                                case SIZE_RESTORED:
                                case SIZE_MAXIMIZED: {

                                		window_w=LOWORD(lParam)+GetSystemMetrics(SM_CXEDGE)+GetSystemMetrics(SM_CXBORDER)+GetSystemMetrics(SM_CXFIXEDFRAME);
                                		window_h=HIWORD(lParam)+statusbarheight+GetSystemMetrics(SM_CYEDGE)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYFIXEDFRAME);
                                        MoveWindow(hListbox,0,0,LOWORD(lParam),HIWORD(lParam)-statusbarheight,FALSE);
                                        MoveWindow(hHostview,0,0,LOWORD(lParam),HIWORD(lParam)-statusbarheight,FALSE);
                                        MoveWindow(hStatusBar,0,HIWORD(lParam)-statusbarheight,LOWORD(lParam),statusbarheight,TRUE);
                                        ShowWindow(hwnd,SW_SHOW);
                                        SetForegroundWindow(hwnd);

                                        return 0;
                                        }
                                break;
                                case SIZE_MINIMIZED:
                                    	ShowWindow(hwnd,SW_HIDE);
                                    	minimized=1;
										return 0;
                                break;
                        }

                break;
                case WM_USER+6:

                          sprintf(caption,"%d/%d %s",connections,max_connections,Rstr(TEXT_CONSOPEN));
                          SendMessage(hStatusBar,WM_SETTEXT,0,(LPARAM)caption);

                                        note.cbSize=sizeof(note);
                                        note.uID=ID_TRAY;
                                        note.hWnd=hwnd;
                                        note.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
                                        note.uCallbackMessage=WM_USER+5;
                                        if (connections>0) {
                                                note.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(MINISHARE_ICON_BUSY));
                                        } else {
                                                note.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(MINISHARE_ICON));

                                        }
                                        char temp666[64];
                                        sprintf(temp666,"MiniShare (%d/%d)",connections,max_connections);
                                        strcpy(note.szTip,temp666);

                                        Shell_NotifyIcon(NIM_MODIFY,&note);



                break;
				case WM_USER+7:
					updatehostview();
				break;
				case WM_USER+8:
				{
					SetTimer(hwnd,WM_DESTROY,wParam,NULL);
				}
				break;

                case WM_USER+5:
                        if (((wParam==ID_TRAY)&&((LOWORD(lParam)==WM_LBUTTONUP))&&(minimized==1))) {
                                        //SendMessage(hwnd,WM_SIZE,SIZE_RESTORED,0);
                                        ShowWindow(hwnd,SW_RESTORE);
                                        //ShowWindow(hwnd,SW_SHOW);

                                        minimized=0;
                                     //   Shell_NotifyIcon(NIM_DELETE,&note);
                        }
                break;
                case WM_COPYDATA: // gets and processes data from command line
                {
	            	PCOPYDATASTRUCT copy=(PCOPYDATASTRUCT)lParam;
	            	switch (copy->dwData)
	            	{
		            	case REMOTE_ADDANDCOPY_FILE:
		            	case REMOTE_ADD_FILE:
		            	addsharedfile((char*)copy->lpData);
		            	updatelist();
		            	if (copy->dwData==REMOTE_ADDANDCOPY_FILE)
		            	{
			            	char tmp[512];
			            	geturlforid(shared_files-1,tmp);
			            	copy_to_clipboard(tmp);
		            	}
		            	break;
		            	case REMOTE_REMOVE_FILE:
		            	deletesharedfilebyfilename((char*)copy->lpData);
		            	updatelist();
		            	break;
		            	case REMOTE_REMOVE_URL:
		            	deletesharedfilebyurl((char*)copy->lpData);
		            	updatelist();
		            	break;
	            	}
	            	return true;
                }
                break;
                case WM_TIMER:
                {
	                if (wParam==WM_DESTROY)
	                {
		             	SendMessage(hwnd,WM_DESTROY,0,0);
	                }
                }
                break;
                case WM_CLOSE:
                {
	                if (MessageBox(hwnd,Rstr(TEXT_QUIT),Rstr(TEXT_QUIT),MB_YESNO|MB_DEFBUTTON2)==IDYES)
                	{
	                	SendMessage(hwnd,WM_DESTROY,0,0);
                	}
                } break;
                case WM_DESTROY:
                {

                			save_config();
                			Shell_NotifyIcon(NIM_DELETE,&note);
                        	PostQuitMessage(0);
             	}
                break;

        }

        if (message!=WM_CLOSE) return DefWindowProc(hwnd, message, wParam, lParam); else return 0;


}

BOOL CALLBACK aboutproc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg){

		case WM_INITDIALOG:
			SendDlgItemMessage(hwnd,ABOOT_LOGO,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)LoadBitmap(hInstance,MAKEINTRESOURCE(LOGO_IMAGE)));
		break;
		case WM_CLOSE:
			EndDialog(hwnd,TRUE);
		break;
		case WM_COMMAND:
			switch(wParam){
				case IDABOUTOK:
					EndDialog(hwnd,TRUE);
					break;
				case 7786:

					ShellExecute(NULL, "open", MINISHARE_URL, NULL, NULL, SW_SHOWNORMAL);
					break;


				}
				break;
			default:	return FALSE;
		}		 return TRUE;


}

int register_class(HINSTANCE hInst,char *classname)
{
		WNDCLASS wndclass;





        wndclass.style=CS_HREDRAW|CS_VREDRAW;
        wndclass.lpfnWndProc=wndproc;
        wndclass.cbClsExtra=0;
        wndclass.cbWndExtra=0;
        wndclass.hInstance=hInst;
        wndclass.lpszMenuName=NULL;
        wndclass.lpszClassName=classname;
        wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
        wndclass.hIcon=LoadIcon(hInst,MAKEINTRESOURCE(MINISHARE_ICON));
        wndclass.hbrBackground=(HBRUSH)GetStockObject (WHITE_BRUSH);
        int cls=RegisterClass(&wndclass);
        if (!cls) return 0;
		wndclass.hbrBackground=(HBRUSH)GetSysColorBrush(COLOR_BTNFACE);

        wndclass.lpszClassName="MiniShareConfig";
		wndclass.lpfnWndProc=configproc;

        cls=RegisterClass(&wndclass);
        if (!cls) return 0;
        return 1;
}

int add_icon(HIMAGELIST F,int res,HINSTANCE hInst)
{
	HICON icon=(HICON)LoadImage(hInst,MAKEINTRESOURCE(res),IMAGE_ICON,0,0,LR_LOADTRANSPARENT|LR_DEFAULTSIZE);
    int r=ImageList_AddIcon(F,(HICON)icon);
    DestroyIcon(icon);
    return r;
}

int init_window(HINSTANCE hInst,int window_w,int window_h,char *classname)
{
	hWnd=CreateWindowEx(WS_EX_ACCEPTFILES,classname,version_string,WS_OVERLAPPEDWINDOW,
                         0,0,window_w,window_h,
                         NULL,NULL,hInst,NULL);
	if (!hWnd) return 0;
        centerwindow(hWnd);


        hListbox = CreateWindowEx (WS_EX_CLIENTEDGE, "SysListView32",0,
        WS_CHILD|LVS_REPORT|WS_VISIBLE|WS_CLIPCHILDREN|LVS_SHAREIMAGELISTS,
        0, 0, 0, 0,
        hWnd, hPopup, hInst, NULL);

        LVCOLUMN lvc;
        lvc.cx=ini_column[2];
        lvc.mask=LVCF_WIDTH|LVCF_TEXT;
        lvc.pszText=Rstr(TEXT_SHAREDPATH);

        SendMessage(hListbox,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);
        lvc.pszText=Rstr(TEXT_FULLPATH);
        lvc.cx=ini_column[1];

        SendMessage(hListbox,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);
        lvc.pszText=Rstr(TEXT_FILENAME);
        lvc.cx=ini_column[0];

        SendMessage(hListbox,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);

        SendMessage(hListbox,LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)sysimagelist);



        hHostview = CreateWindowEx (WS_EX_CLIENTEDGE, "SysListView32",0,
        WS_CHILD|LVS_REPORT|WS_VISIBLE|WS_CLIPCHILDREN,
        0, 0, 300, 200,
        hWnd, hPopup, hInst, NULL);



        SendMessage(hHostview,
                        LVM_SETEXTENDEDLISTVIEWSTYLE,0,
                        LVS_EX_FULLROWSELECT);
     	SendMessage(hListbox,
                        LVM_SETEXTENDEDLISTVIEWSTYLE,0,
                        LVS_EX_FULLROWSELECT);

        lvc.cx=ini_column[6];
        lvc.mask=LVCF_WIDTH|LVCF_TEXT;
        lvc.pszText=Rstr(TEXT_SENTDATA);

        SendMessage(hHostview,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);
        lvc.pszText=Rstr(TEXT_DOWNLOADING);
        lvc.cx=ini_column[5];

        SendMessage(hHostview,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);
        lvc.pszText=Rstr(TEXT_HOST);
        lvc.cx=ini_column[4];

        SendMessage(hHostview,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);
        lvc.pszText=Rstr(TEXT_CONNECTION);
        lvc.cx=ini_column[3];

        SendMessage(hHostview,LVM_INSERTCOLUMN,0,(WPARAM)&lvc);

        FilesImageList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLORDDB | ILC_MASK, 0, 4096);
        icon_send=add_icon(FilesImageList,LIST_SEND,hInst);
        icon_state[0]=add_icon(FilesImageList,LIST_HIDDEN,hInst);
        icon_state[1]=add_icon(FilesImageList,LIST_VISIBLE,hInst);

        SendMessage(hHostview,LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)FilesImageList);
        SendMessage(hListbox,LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)FilesImageList);

        HWND hHeaderCtrl = (HWND)SendMessage(hListbox,LVM_GETHEADER,0,0);
  		RECT r;
  		GetClientRect(hHeaderCtrl,&r);


        ShowWindow(hHostview,SW_HIDE);
        //ShowWindow(hListbox,SW_HIDE);





        hDrag = CreateWindowEx (0, "STATIC",Rstr(TEXT_DRAGDROP),
        WS_CHILD,
        4, r.bottom+4, 400, 14,
        hWnd, (HMENU)0, hInst, NULL);
        HFONT hFont = CreateFont(8 *(-1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,(LPCTSTR) "MS Sans Serif");
    	SendMessage(hDrag,WM_SETFONT,(WPARAM) hFont,(LPARAM)TRUE);


    	hStatusBar = CreateWindow(STATUSCLASSNAME, "",
               WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
        RECT rect;
    	GetWindowRect(hStatusBar, &rect);
    	statusbarheight=rect.bottom-rect.top;
    	if (!start_minimized&&!temp_start_minimized) ShowWindow(hWnd,SW_SHOW);
    	return 1;
}
