#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 22

void display_page(FILE *file) {
    int ch;
    int lines = 0;

    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);

        if (ch == '\n') {
            lines++;
        }

        if (lines == PAGE_SIZE) {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    display_page(file);

    fclose(file);

    return 0;
}
