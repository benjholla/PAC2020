#include <stdio.h>
#include <strings.h>
#include <time.h>
#include <cstring>

FILE *logfile;

void addtolog(int i,char *msg) {
        if (logfile!=NULL) {
                char tmp[1800];
                time_t r=time(NULL);
                char tmp2[1300];
                strcpy(tmp2,asctime(localtime(&r)));
                tmp2[strlen(tmp2)-1]=0;
                sprintf(tmp,"%s (%d) %s\n",tmp2,i,msg);
                fputs(tmp,logfile);
                fflush(logfile);
        }
}

