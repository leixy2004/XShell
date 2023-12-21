#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "defined_constant.h"
#include "parse_command.h"


int main(int argc,char *argv[]) {
    char command[MAX_COMMAND_LENGTH],tmp[MAX_COMMAND_LENGTH];
    int opt;
    int deli=EOF;
    while ((opt=getopt(argc,argv,"d:"))!=-1) {
        switch (opt) {
            case 'd':{
                printf("Option 'd' with argument: %s\n", optarg);
                int dlen=strlen(optarg);
                if (dlen==1) deli=optarg[0];
                if (dlen==2) {
                    if (optarg[0]!='\\') {
                        perror("optarg");
                        return 1;
                    }
                    switch (optarg[1])
                    {
                    case 'n':
                        deli='\n';
                        break;
                    case 't':
                        deli='\t';
                        break;
                    case '\\':
                        deli='\\';
                        break;
                    default:
                        break;
                    }
                }
                break;
            }
            case '?':
                fprintf(stderr, "Unknown option: %c\n", optopt);
                return 1;
        }
    }
    strcpy(command,argv[optind]);
    int len=strlen(command);
    command[len++]=' ';
    if (deli==EOF) {
        while (scanf("%s",tmp)!=EOF) {
            int tlen=strlen(tmp);
            strcpy(command+len,tmp);
            len+=tlen;
            command[len++]=' ';
        }
    } else {
        char c;
        bool add_division=false;
        while ((c=getchar())!=EOF) {
            if (c==deli) {
                if (!add_division) {
                    command[len++]=' ';
                    add_division=true;
                }
            } else {
                add_division=false;
                command[len++]=c;
            }
        }
    }
    parse_command(command);

    return 0;
}