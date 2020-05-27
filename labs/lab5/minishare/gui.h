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

void centerwindow(HWND hwnd);
BOOL CALLBACK aboutproc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int register_class(HINSTANCE hInst,char *classname);
int init_window(HINSTANCE hInst,int window_w,int window_h,char *classname);
void updatelist();
void copy_to_clipboard(char *data);
