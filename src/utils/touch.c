#include <stdio.h>
#include <stdlib.h>
#include <utime.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        const char *filename = argv[i];

        // 尝试获取文件信息
        struct stat file_stat;
        if (stat(filename, &file_stat) == 0) {
            // 文件存在，更新访问和修改时间
            utime(filename, NULL);
        } else {
            // 文件不存在，创建一个空文件
            FILE *file = fopen(filename, "w");
            if (file == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }
            fclose(file);
        }
    }

    return 0;
}
