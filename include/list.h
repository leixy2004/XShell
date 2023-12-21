#pragma once

typedef struct list_node {
    int value;
    struct list_node *next;
} list_node;

typedef struct list {
    list_node *head;
    int size;
} list;

list * list_build();
void list_delete(list *l);
void list_push(list *l,int value);
int *list_front(list *l);
void list_pop(list *l);
