#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#define PAGE_SIZE 22
#define BUFFER_MAX 256
static list* saved_size;
bool display_page(FILE *file) {
    static char buffer[BUFFER_MAX];
    clear();
    int now_size=0;
    for (int i=0;i<PAGE_SIZE;i++) {
        if (fgets(buffer,BUFFER_MAX,file)==NULL) {
            now_size+=strlen(buffer);
            list_push(saved_size,now_size);
            refresh();
            return false;
        }
        now_size+=strlen(buffer);
        printw("%s\n",buffer);
    }
    list_push(saved_size,now_size);
    refresh();
    return true;
}

int main(int argc,char *argv[]) {
    FILE *file=fopen(argv[1],"r");
    if (file==NULL) {
        perror("fopen");
        return 1;
    }
    saved_size=list_build();
    initscr();  // 初始化 ncurses
    noecho();
    raw();      // 禁用行缓冲，启用原始模式
    keypad(stdscr, TRUE);  // 启用功能键
    clear();
    int ch=0;
    bool flag=display_page(file);
    while (flag) {
        ch=getch();
        switch (ch)
        {
        case ' ':{
            flag=display_page(file);
            break;
        }
        case 'b':{
            if (saved_size->size>=2) {
                int size1=*list_front(saved_size);
                list_pop(saved_size);
                int size2=*list_front(saved_size);
                list_pop(saved_size);
                fseek(file,-(size1+size2),SEEK_CUR);
                flag=display_page(file);
            }
            break;
        }
        case 'q':{
            flag=false;
            break;
        }
        case 'g':{
            // bug: unaligned fastbin chunk detected 3
            list_delete(saved_size);
            list_build(saved_size);
            fseek(file,0,SEEK_SET);
            flag=display_page(file);
        }

        default:
            break;
        }
    }

    endwin();  // 结束 ncurses
    fclose(file);
    return 0;
}
