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

typedef struct
{
	int id;
	char param[20],param_short[20];
	int type;
} PARAMETER;

#define PARAM_NONE 0
#define PARAM_STR 1
#define PARAM_INT 2


#define CMDLINE_ADD 1
#define CMDLINE_REMOVE 2
#define CMDLINE_MINIMIZE 3

#define CMDLINE_DIE 5
#define CMDLINE_REMOVEURL 6
#define CMDLINE_ADDANDCOPY 7
#define CMDLINE_FORCENEWINSTANCE 8

#define CMDLINE_USELIST 9
#define CMDLINE_OLDENGINE 11

int process_command_line(HINSTANCE hInst,int argc,char **argv);
