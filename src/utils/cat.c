#include <stdio.h>

int main(int argc, char *argv[]) {
    // 如果未提供文件参数，则从标准输入读取
    if (argc == 1) {
        int ch;
        while ((ch = getchar()) != EOF) {
            putchar(ch);
        }
    } else {
        // 逐个打开并读取文件
        for (int i = 1; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");

            if (file == NULL) {
                perror("fopen");
                continue;  // 继续处理下一个文件
            }

            int ch;
            while ((ch = fgetc(file)) != EOF) {
                putchar(ch);
            }

            fclose(file);
        }
    }

    return 0;
}
