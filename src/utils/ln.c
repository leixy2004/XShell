#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

int main(int argc,char * argv[]) {
    int opt;
    bool is_symbolic_link=false;
    while ((opt=getopt(argc,argv,"s")!=-1)) {
        switch (opt)
        {
        case 's':
            is_symbolic_link=true;
            break;
        
        default:
            perror("Format Error");
            break;
        }
    }
    const char *file_name=argv[optind];
    const char *link_name=argv[optind+1];
    if (is_symbolic_link) {
        if (symlink(file_name,link_name)==-1) {
            perror("symlink");
            exit(EXIT_FAILURE);
        }
        printf("Symbolic link created successfully: %s -> %s\n", link_name, file_name);
    } else {
        if (link(file_name,link_name)==-1) {
            perror("link");
            exit(EXIT_FAILURE);
        }
        printf("Hard link created successfully: %s -> %s\n", link_name, file_name);
    }
    return 0;
}