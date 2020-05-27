#include <windows.h>
#include "minishare.h"
#include "globals.h"

int edit_file=0;

BOOL CALLBACK editfileproc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg){

		case WM_INITDIALOG:
			SetDlgItemText(hwnd,EDITPATH,sharedfiles[edit_file].filename);
			SetDlgItemText(hwnd,EDITURL,sharedfiles[edit_file].shared_as);
		break;
		case WM_CLOSE:
			EndDialog(hwnd,TRUE);
		break;
		case WM_COMMAND:
			switch(wParam){
				case EDITOK:
					char tmp[513];
					GetDlgItemText(hwnd,EDITURL,tmp,512);
					sharedfiles[edit_file].shared_as=(char*)realloc(sharedfiles[edit_file].shared_as,strlen(tmp)+1);
					strcpy(sharedfiles[edit_file].shared_as,tmp);
					
					GetDlgItemText(hwnd,EDITPATH,tmp,512);
					sharedfiles[edit_file].filename=(char*)realloc(sharedfiles[edit_file].filename,strlen(tmp)+1);
					strcpy(sharedfiles[edit_file].filename,tmp);
				
					EndDialog(hwnd,TRUE);
					break;
				case EDITCANCEL:
					EndDialog(hwnd,TRUE);
					break;
				}
				break;
			default:	return FALSE;
		}		 return TRUE;


}
