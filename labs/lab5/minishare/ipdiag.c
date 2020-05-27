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

#include <windows.h>

int selip=0;
char cipt[256][17];
int ips=0;
#define IP_BOX 8080
#define IPOK 8081
                
#define IPCOMBO 8082
int CALLBACK ipproc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg){
		case WM_INITDIALOG:
		{
			int i;
			for (i=0;i<ips;i++)
				SendDlgItemMessage(hwnd,IPCOMBO,CB_ADDSTRING,0,(LPARAM)&cipt[i]);
		}
		break;
		case WM_CLOSE:
			selip=SendDlgItemMessage(hwnd,IPCOMBO,CB_GETCURSEL,0,0);
			EndDialog(hwnd,TRUE);		
		break;
		case WM_COMMAND:
			switch(wParam){
				case IPOK:		
					
					selip=SendDlgItemMessage(hwnd,IPCOMBO,CB_GETCURSEL,0,0);
					EndDialog(hwnd,TRUE);		
					break;
				}	
				break;
			default:	return FALSE;
		}		 return TRUE;	
	
		
}
