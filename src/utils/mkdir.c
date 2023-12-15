#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s directory1 [directory2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        const char *dirname = argv[i];

        // 创建目录
        if (mkdir(dirname, 0777) == -1) {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
