#include <windows.h>
#include <stdio.h>
#include "files.h"
#include "auth.h"
#include "server.h"

extern HWND hWnd,hHostview;
extern HINSTANCE hInstance;
extern char server_domain[80],con_server_domain[80];
extern char *shared[MAX_SHARED_FILES+1];
extern char *shared_real[MAX_SHARED_FILES+1];
extern char logfilename[2600],logfilename_f[260],motdfilename[2600],css_path[2600];
extern char sharepage[128],server_name[256],minishare_work_path[256],filelistfile[256];

extern SHAREDFILE sharedfiles[MAX_SHARED_FILES+1];

extern int max_connections;
extern int shared_files;
extern int connections;
extern int infopage;
extern int showlist;
extern int server_running,start_minimized;
extern int random_letters;
extern int temp_start_minimized,die_in;
extern int ini_column[8],window_w,window_h;
extern FILE *logfile;
//extern config_param config_parameters[config_parameter_count];
extern int tot_users;
extern int old_engine;
extern user users[MAXUSERS];

extern int config_open;
extern char text_bytes[32],text_kb[32],text_mb[32],text_gb[32],text_sec[32];

extern HWND hListbox,hConfigOK,hConfigCancel,hDrag;
extern HINSTANCE hInstance;
extern int server_port;
extern char cipt[256][17];
extern int ips;
extern int selip;
extern SHAREDFILE sharedfiles[MAX_SHARED_FILES+1];

extern char css_path_f[256];
extern char server_domain[80],con_server_domain[80];
extern char *shared[MAX_SHARED_FILES+1];
extern char *shared_real[MAX_SHARED_FILES+1];
extern char logfilename[2600],logfilename_f[260],motdfilename[2600],motdfilename_f[2600],css_path[2600];
extern char sharepage[128],server_name[256],minishare_work_path[256];
extern HWND hWnd,hHostview;
extern HINSTANCE hInstance;

extern int max_connections;
extern int shared_files;
extern int connections;
extern int infopage;
extern int showlist;
extern int server_running,start_minimized;
extern int random_letters;
extern int ini_column[8],window_w,window_h;
extern FILE *logfile;

extern char mimetype[MAX_MIMES][64];
extern char mimetype_ext[MAX_MIMES][8];
extern int mimetypes;

extern int edit_file;

