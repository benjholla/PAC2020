#include <windows.h>
#include "minishare.h"
#include "globals.h"

static int selected_login=CB_ERR;

BOOL CALLBACK adduserproc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg){

			case WM_INITDIALOG:
				
			break;
			case WM_CLOSE:
				EndDialog(hwnd,FALSE);
			break;
			case WM_COMMAND:
				switch(wParam){
					case ADDUSEROK:
						char tmp[40],tmp2[40],lp[32*2+2];
						GetDlgItemText(hwnd,ADDUSERNAME,tmp,32);
						GetDlgItemText(hwnd,ADDUSERPASS,tmp2,32);
						if (strlen(tmp)>0&&strlen(tmp2)>0){
							sprintf(lp,"%s:%s",tmp,tmp2);
							add_user(lp);
							EndDialog(hwnd,TRUE);
						} else EndDialog(hwnd,FALSE);
					 	break;
					case ADDUSERCANCEL:
						EndDialog(hwnd,FALSE);
						break;
					}
					break;
			default: return FALSE;
		}		 
		
		return TRUE;
	
}

BOOL CALLBACK edituserproc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg){

			case WM_INITDIALOG:
				int i;
				for (i=0;i<tot_users;i++)
					SendDlgItemMessage(hwnd,USEREDITUSER,CB_ADDSTRING,0,(LPARAM)&users[i].login);
			break;
			case WM_CLOSE:
				save_users();
				EndDialog(hwnd,TRUE);
			break;
			case WM_COMMAND:
				switch(HIWORD(wParam)) {
					case CBN_SELENDOK: {
						LRESULT sel=SendDlgItemMessage(hwnd,USEREDITUSER,CB_GETCURSEL,0,0);
						if (sel!=CB_ERR) { 
							SetDlgItemText(hwnd,USEREDITPASSWORD,users[sel].passwd);
							selected_login=sel;
						}
					}
					break;
				}
				switch(wParam){
					case USEREDITMODIFY:
						if (selected_login!=CB_ERR) {
							GetDlgItemText(hwnd,USEREDITPASSWORD,users[selected_login].passwd,32);	
						}
						break;
					case USEREDITADD:
						if (DialogBox(hInstance,MAKEINTRESOURCE(ADD_USER_BOX),hwnd,(DLGPROC) MakeProcInstance((FARPROC)adduserproc,hInstance))==TRUE)
							SendDlgItemMessage(hwnd,USEREDITUSER,CB_ADDSTRING,0,(LPARAM)&users[tot_users-1].login);
						break;
					case USEREDITDELETE:
						if (selected_login!=CB_ERR) {
							delete_user(selected_login);
							SendDlgItemMessage(hwnd,USEREDITUSER,CB_DELETESTRING,selected_login,0);
						}
						break;
					case USEREDITCLOSE:
						save_users();
						EndDialog(hwnd,TRUE);
						break;
					}
					break;
			default: return FALSE;
		}		 
		
		return TRUE;


}
