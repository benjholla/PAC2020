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
#define VERSION "MiniShare 1.4.1"
#define NIGHTLY 0

#define _WIN32_IE       0x0300

#define bool int
#define true 1
#define false 0

#if (NIGHTLY==0)
#define version_string VERSION
#else
#define version_string VERSION" preview ("__DATE__")"
#endif

#define DOCTYPE "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">"
#define LINKCSS "<link rel=\"stylesheet\" href=\"/minishare.css\" type=\"text/css\">"

#define GUI
#define MAX_SHARED_FILES 32768
#define MINISHARE_URL "http://minishare.sourceforge.net/"
#define MAX_MIMES 512
#define SEND_BUFFER_SIZE 8192
#define config_parameter_count 16

#define IDM_COPYURL 5555

#define IDM_REMOVEFILE 5556
#define IDM_COPYINDEX 5557
#define IDM_LOG 5558
#define IDM_ABOUT 5559

#define ID_LISTBOX 1
#define ID_TRAY 666

#define MINISHARE_ICON 4242
#define MINISHARE_ICON_BUSY 4243
#define MENU_ABOUT 7777
#define MENU_CONFIG 7778
#define MENU_COPY 7779
#define MENU_REMOVE 7780
#define MENU_INDEX 7781
#define MENU_LOG 7782
#define ABOUT_BOX 7783
#define IDABOUTOK 7784
#define ABOOT_LOGO 7787
#define LOGO_IMAGE 7788
#define MENU_OPEN 7789
#define MENU_DIR 7790

#define IP_BOX 8080
#define IPOK 8081
#define IPCOMBO 8082

#define EDIT_FILE_BOX 8090
#define EDITPATH 8091
#define EDITURL 8092
#define EDITOK 8093
#define EDITCANCEL 8094

#define EDIT_USER_BOX 9000
#define USEREDITUSER 9002
#define USEREDITPASSWORD 9003
#define USEREDITADD 9004
#define USEREDITDELETE 9005
#define USEREDITCLOSE 9006
#define USEREDITMODIFY 9007

#define ADD_USER_BOX 10000
#define ADDUSERNAME 10001
#define ADDUSERPASS 10002
#define ADDUSEROK 10003
#define ADDUSERCANCEL 10004


#define ABOOT_LOGO 7787
#define LOGO_IMAGE 7788
#define MENU_CONNECTION 7791
#define MENU_SHOWHIDE 7792
#define LIST_SEND 7793
#define LIST_HIDDEN 7794
#define LIST_VISIBLE 7795
#define ID_TRAY 666
#define ID_LISTBOX 1

#define IDCONFIGOK 6110
#define IDCONFIGCANCEL 6111
#define IDCONFIGAPPLY 6112



#define IDM_COPYURL 5555
#define IDM_REMOVEFILE 5556
#define IDM_COPYINDEX 5557
#define IDM_LOG 5558
#define IDM_ABOUT 5559




#define IDM_CONFIG 5560
#define IDM_ADDFILE 5561
#define IDM_ADDDIR 5562
#define IDM_SHOWFILES 5563
#define IDM_SHOWCONNECTIONS 5564
#define IDM_SHOWHIDE 5565
#define IDM_KICK 5566
#define IDM_KICKBAN 5567
#define IDM_ADDLOGIN 5568

#define IDM_EDITFILE 5569

#define IDM_AUTH_NONE 15000 // attn




#define REMOTE_ADD_FILE 1
#define REMOTE_REMOVE_FILE 2
#define REMOTE_REMOVE_URL 3
#define REMOTE_ADDANDCOPY_FILE 4

#define	TEXT_SELECTDIR 20000
#define TEXT_MAXSHAREDEXCEEDED 20001
#define	TEXT_WARNING 20002
#define TEXT_ADDFILES 20003
#define	TEXT_ADDDIR 20004
#define	TEXT_COPYURL 20005
#define	TEXT_REMOVE 20006
#define TEXT_COPYINDEX 20007
#define TEXT_OPENLOG 20008
#define TEXT_CONFIG 20009
#define TEXT_ABOUT 20010
#define TEXT_CONSOPEN 20011
#define TEXT_OK 20012
#define TEXT_CANCEL 20013
#define TEXT_APPLY 20014
#define TEXT_RESTARTSERVER 20015
#define TEXT_DRAGDROP 20016
#define TEXT_SHAREDPATH 20017
#define TEXT_FULLPATH 20018
#define TEXT_FILENAME 20019
#define TEXT_SERVERCONF 20020
#define TEXT_SERVERPORT 20021
#define TEXT_MAXCONS 20022
#define TEXT_SHAREOPTS 20023
#define TEXT_NUMRNDCHARS 20024
#define TEXT_FIXEDDOMAIN 20025
#define TEXT_INDEXPATH 20026
#define TEXT_SHOWLIST 20027
#define TEXT_INDEXTITLE 20028
#define TEXT_PATHS 20029
#define TEXT_MOTD 20030
#define TEXT_LOGPATH 20031
#define TEXT_CSSPATH 20032
#define TEXT_INTERFACE 20033
#define TEXT_STARTMINIMIZED 20034
#define TEXT_MINISHARECONFIG 20035
#define TEXT_BYTES 20036
#define TEXT_KB 20037
#define TEXT_MB 20038
#define TEXT_GB 20039
#define TEXT_ERROR 20040
#define TEXT_CANTBIND 20041
#define TEXT_FILES 20042
#define TEXT_TOTAL 20043
#define TEXT_CONNECTION 20044
#define TEXT_HOST 20045
#define TEXT_DOWNLOADING 20046
#define TEXT_SENTDATA 20047
#define TEXT_SHOWFILES 20048
#define TEXT_SHOWCONNECTIONS 20049
#define TEXT_SEC 20050
#define TEXT_SHOWHIDE 20051
#define TEXT_QUIT 20052
#define TEXT_KICK 20053
#define TEXT_KICKBAN 20054
#define TEXT_AUTHMENU 20055
#define TEXT_NONE 20056
#define TEXT_ADDLOGIN 20057
#define TEXT_EDITFILE 20058
