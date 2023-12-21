#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]) {
    int c;
    if (argc==1) {
        while ((c=fgetc(stdin))!=EOF) {
            fputc(c,stdout);
        }
    } else {
        for (int i=1;argv[i]!=NULL;i++) {
            FILE *f=fopen(argv[i],"r");
            if (f==NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }
            while ((c=fgetc(f))!=EOF) {
                fputc(c,stdout);
            }
            fclose(f);
        }
    }
    return 0;
}