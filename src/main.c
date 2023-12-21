#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <pwd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "ANSI_color_codes.h"
#include "parse_command.h"
#include "defined_constant.h"


char *update_prompt() {
    static char hostname[HOST_NAME_MAX];
    static char cwd[PATH_MAX];
    static char prompt[PATH_MAX+HOST_NAME_MAX+32];
    uid_t uid=getuid();
    struct passwd *pw=getpwuid(uid);
    if (gethostname(hostname,sizeof(hostname))==-1
     ||  getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("update_prompt");
        exit(EXIT_FAILURE);
    }

    snprintf(prompt,sizeof(prompt),"(FAKE) " BHGRN "%s@%s" CRESET ":" BHBLU "%s" CRESET "%c ",
                                            pw->pw_name,hostname,cwd,(pw->pw_uid==0 ? '#' : '$'));
    return prompt;
}

void set_environment() {
    char *name="PATH";
    char value[MAX_VAR_PATH]="/home/deblei/XShell/build/linux/x86_64/debug:";
    char *current_value=getenv(name);
    if (current_value==NULL) {
        perror("getenv");
        exit(EXIT_FAILURE);
    }
    strcat(value,current_value);
    printf("Now set $PATH:%s\n",value);
    if (setenv(name,value,true)==-1) {
        perror("setenv");
        exit(EXIT_FAILURE);
    }
}

void sigint_handler(int signum) {
    // printf("Nothing happened\n");
}

int main() {
    char *input;
    signal(SIGINT,sigint_handler);
    set_environment();
    using_history();
    printf("Hello there!\n");
    while ((input=readline(update_prompt()))) {
        // *strchr(input,'\n')='\0';
        printf("INPUT:\"%s\"\n",input);
        printf("------------OUTPUT------------\n");
        parse_command(input);
        printf("--------------END-------------\n");

        free(input);
        fflush(stdout);
    }
    printf("It's over, Anakin! I have the high ground!\n");
    return 0;
}
