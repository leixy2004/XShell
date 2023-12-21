#include <stdlib.h>
#include <stdio.h>
#include "list.h"


list_node * node_create(int value,list_node *next) {
    list_node *new_node=malloc(sizeof(list_node));
    if (new_node==NULL) {
        perror("node_create malloc");
        return NULL;
    }
    new_node->value=value;
    new_node->next=next;
    return new_node;
}

list * list_build() {
    list *new_list=malloc(sizeof(list));
    new_list->head=node_create(-1,NULL);
    new_list->size=0;
    return new_list;
}


void list_delete(list *l) {
    if (l==NULL) return;
    list_node * p=l->head;
    while (p) {
        list_node *q=p->next;
        free(p);
        p=q;
    }
    free(l);
}

void list_push(list *l,int value) {
    l->head=node_create(value,l->head);    
    l->size++;
}

int *list_front(list *l) {
    return &(l->head->value);
}

void list_pop(list *l) {
    list_node *waste=l->head;
    l->head=l->head->next;
    free(waste);
}