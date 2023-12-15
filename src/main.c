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
#include "builtin_command.h"
#include "ANSI_color_codes.h"
#define MAX_COMMANDS 16
#define MAX_ARGS 16
#define MAX_COMMAND_LENGTH 128

extern char **environ;

int execute_command(char *command, int input_fd, int output_fd) {
    printf("execute_command(%s,%d,%d)\n",command,input_fd,output_fd);
    
    char *args[MAX_ARGS];
    char *token, *rest=command;
    int arg_count = 0;

    while ((token = strtok_r(rest, " \t", &rest)) != NULL) {
        args[arg_count] = token;
        printf("args[%d]:%s\n",arg_count,args[arg_count]);
        arg_count++;
    }

    args[arg_count] = NULL;
    if (check_builtin_command(arg_count,args)) return 0;
    pid_t pid = fork();

    if (pid == 0) {
        // Child process

        if (input_fd != STDIN_FILENO) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_fd != STDOUT_FILENO) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process

        if (input_fd != STDIN_FILENO) {
            close(input_fd);
        }

        if (output_fd != STDOUT_FILENO) {
            close(output_fd);
        }
        return pid;
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

void parse_by_redirect_and_pipe(int command_count,char *commands[MAX_COMMANDS]) {
    printf("parse_by_redirect_and_pipe\n");
    int pipes_fd[MAX_COMMANDS][2];
    pid_t pids[MAX_COMMANDS];
    for (int i=0;i<command_count-1;i++) {
        if (pipe(pipes_fd[i])==-1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        printf("pipes_fd[%d]={%d,%d}\n",i,pipes_fd[i][0],pipes_fd[i][1]);
    }
    for (int i=0;i<command_count;i++) {
        char * command=commands[i];
        int input_fd=(i==0 ? STDIN_FILENO : pipes_fd[i-1][0]);
        int output_fd=(i==command_count-1 ? STDOUT_FILENO : pipes_fd[i][1]);
        pids[i]=execute_command(command,input_fd,output_fd);
        printf("pids[%d]=%d\n",i,pids[i]);
    }
    for (int i=0;i<command_count;i++) {
        int status=0;
        // wait(NULL);
        if (pids[i]) waitpid(pids[i],&status,0);
        if (true) {
            printf("Process %d exit with %d\n",pids[i],status);
        }
    }
    printf("completed.\n");
    for (int i=0;i<command_count-1;i++) {
        close(pipes_fd[i][0]);
        close(pipes_fd[i][1]);
    }
}

void parse_by_pipe(char *multi_command) {
    printf("parse_by_pipe\n");

    char *token;

    char *commands[MAX_COMMANDS];
    int command_count = 0;

    // Check for commands
    while ((token = strtok_r(multi_command, "|", &multi_command)) != NULL) {
        commands[command_count] = token;
        printf("by_pipe[%d]:%s\n",command_count,commands[command_count]);
        command_count++;
    }

    parse_by_redirect_and_pipe(command_count,commands);
}

void parse_by_semicolon(char *command) {
    printf("parse_by_semicolon\n");

    char *token, *rest = command;
    while ((token = strtok_r(rest, ";", &rest)) != NULL) {
        parse_by_pipe(token);
    }
}

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
    #define MAX_VAR_PATH 4096
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

int main() {
    char *input;
    set_environment();
    using_history();
    printf("Hello there!\n");
    while ((input=readline(update_prompt()))) {
        // *strchr(input,'\n')='\0';
        char *ptr;
        while ((ptr=strchr(input,'\n'))) *ptr=' ';
        add_history(input);
        printf("INPUT:\"%s\"\n",input);
        printf("------------OUTPUT------------\n");
        parse_by_semicolon(input);
        printf("--------------END-------------\n");

        free(input);
        fflush(stdout);
    }
    printf("It's over, Anakin! I have the high ground!\n");
    return 0;
}
