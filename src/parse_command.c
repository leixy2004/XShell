#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include "builtin_command.h"
#include "parse_command.h"
#include "defined_constant.h"


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

void parse_by_redirect_and_pipe(int command_count,char *commands[MAX_COMMANDS],bool is_background) {
    int dev_null=0;
    if (is_background) {
        dev_null=open("/dev/null",O_WRONLY);
    }
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
        char *ptr;
        int input_fd=(i==0 ? STDIN_FILENO : pipes_fd[i-1][0]);
        int output_fd=(i==command_count-1 ? STDOUT_FILENO : pipes_fd[i][1]);
        if (is_background && output_fd==STDOUT_FILENO) {
            output_fd=dev_null;
        } 
        if ((ptr=strchr(command,'<'))) {
            printf("Find <!\n");
            if (input_fd!=STDIN_FILENO) {
                perror("input_file");
            } else {
                char input_filename[FILENAME_MAX];
                sscanf(ptr+1,"%s",input_filename);
                input_fd=open(input_filename,O_RDONLY);
                if (input_fd==-1) {
                    perror("input_file_name");
                } else {
                    *ptr=' ';
                    while (*ptr && *ptr==' ') ptr++;
                    while (*ptr && *ptr!=' ') {
                        *ptr=' ';
                        ptr++;
                    }
                }
            }
        }
        if ((ptr=strchr(command,'>'))) {
            printf("Find >!\n");
            if (output_fd!=STDOUT_FILENO) {
                perror("output_file");
            } else {
                char output_filename[FILENAME_MAX];
                sscanf(ptr+1,"%s",output_filename);
                output_fd=open(output_filename,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (output_fd==-1) {
                    perror("output_file_name");
                } else {
                    *ptr=' ';
                    while (*ptr && *ptr==' ') ptr++;
                    while (*ptr && *ptr!=' ') {
                        *ptr=' ';
                        ptr++;
                    }
                }
            }
        }
        pids[i]=execute_command(command,input_fd,output_fd);
        printf("pids[%d]=%d\n",i,pids[i]);
    }
    if (!is_background) 
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
    if (is_background) {
        close(dev_null);
    }
}

void parse_by_pipe(char *multi_command,bool is_background) {
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

    parse_by_redirect_and_pipe(command_count,commands,is_background);
}

void parse_by_semicolon(char *command,bool is_background) {
    printf("parse_by_semicolon\n");

    char *token, *rest = command;
    while ((token = strtok_r(rest, ";", &rest)) != NULL) {
        parse_by_pipe(token,is_background);
    }
}

void parse_command(char *command) {
    char *ptr;
    bool is_background;
    while ((ptr=strchr(command,'\n'))) *ptr=' ';
    if ((ptr=strchr(command,'&'))) {
        *ptr=' ';
        is_background=true;
    } else {
        is_background=false;
    }
    parse_by_semicolon(command,is_background);
}