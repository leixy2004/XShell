#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_file_info(const char *filename) {
    struct stat file_stat;

    // 获取文件属性
    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        return;
    }

    // 打印文件信息
    printf("%s", (S_ISDIR(file_stat.st_mode) ? "d" : "-"));
    printf("%c%c%c%c%c%c%c%c%c ",
           (file_stat.st_mode & S_IRUSR) ? 'r' : '-',
           (file_stat.st_mode & S_IWUSR) ? 'w' : '-',
           (file_stat.st_mode & S_IXUSR) ? 'x' : '-',
           (file_stat.st_mode & S_IRGRP) ? 'r' : '-',
           (file_stat.st_mode & S_IWGRP) ? 'w' : '-',
           (file_stat.st_mode & S_IXGRP) ? 'x' : '-',
           (file_stat.st_mode & S_IROTH) ? 'r' : '-',
           (file_stat.st_mode & S_IWOTH) ? 'w' : '-',
           (file_stat.st_mode & S_IXOTH) ? 'x' : '-');

    printf("%ld ", (long)file_stat.st_nlink);

    struct passwd *pwd = getpwuid(file_stat.st_uid);
    printf("%s ", pwd ? pwd->pw_name : "unknown");

    struct group *grp = getgrgid(file_stat.st_gid);
    printf("%s ", grp ? grp->gr_name : "unknown");

    printf("%lld ", (long long)file_stat.st_size);

    char time_buf[64];
    strftime(time_buf, sizeof(time_buf), "%b %e %H:%M", localtime(&file_stat.st_mtime));
    printf("%s ", time_buf);

    printf("%s\n", filename);
}

void list_directory(const char *path, int show_details) {
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // 忽略当前目录和上级目录
        if (entry->d_name[0] == '.') {
            continue;
        }

        if (show_details) {
            print_file_info(entry->d_name);
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    int show_details = 0;  // 是否显示详细信息

    int opt;
    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
            case 'l':
                show_details = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // 处理剩余的非选项参数
    for (int i = optind; i < argc; i++) {
        list_directory(argv[i], show_details);
    }

    // 如果没有指定目录，则默认使用当前目录
    if (optind == argc) {
        list_directory(".", show_details);
    }

    return 0;
}
