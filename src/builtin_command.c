#include "builtin_command.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#define MAX_COMMAND 16
#define MAX_BUILTIN 5

char command_list[MAX_BUILTIN][MAX_COMMAND]={"exit","cd","pwd","kill"};

bool check_builtin_command(int argc,char *argv[]) {
    int flag=-1;
    for (int i=0;i<MAX_BUILTIN;i++) {
        if (strcmp(argv[0],command_list[i])!=0) continue;
        flag=i;
    }
    if (flag==-1) return false;
    switch (flag)
    {
    case 0:{
        int status=0;
        if (argc==2) status=atoi(argv[1]);
        exit(status); 
        break;
    }
    case 1:{
        if (chdir(argv[1])==-1)
            printf("No dir here.\n");
        break;
    }
    case 2:{
        char path[PATH_MAX];
        getcwd(path,PATH_MAX);
        printf("%s\n",path);
        break;
    }
    case 3:{
        break;
    }
    default:
        break;
    }
}
