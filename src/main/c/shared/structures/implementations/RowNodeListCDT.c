#include "../../Type.h"
#include "../RowNodeListADT.h"
#include <stdlib.h>

typedef struct RowNode{
    RowData data;
    struct RowNode * next;
} RowNode;

typedef struct RowNodeListCDT {
    RowNode * head;
    RowNode * last;
    RowNode * itActual;
} RowNodeListCDT;

RowNodeListADT newRowNodeList(void){
    RowNodeListADT ans = calloc(1,sizeof(RowNodeListCDT));
    return ans;
}

void createRowNode(RowNodeListADT rnl, RowData data){
    if(rnl == NULL){
        return;
    }
    RowNode * node = malloc(sizeof(RowNode));
    node->data = data;
    node->next = NULL;
    RowNode* last = rnl->last;
    if(rnl->head == NULL && rnl->last == NULL){
        rnl->head = node;
    } else {
        rnl->last->next = node;
    }
    rnl->last = node;
    rnl->itActual = NULL;
}	

void freeRowNodeList(RowNodeListADT rnl){
    if(rnl == NULL){
        return;
    }

    RowNode * actual;
    actual = rnl->head;
    while(actual != NULL){
        RowNode * aux = actual->next;
        free(actual);
        actual = aux;
    }
    free(rnl);
}

void beginIteration(RowNodeListADT rnl){
    if(rnl == NULL || rnl->head == NULL){
        return;
    }
    rnl->itActual = rnl->head;
}

RowData next(RowNodeListADT rnl){
    if(rnl == NULL || !hasNext(rnl)){
        return (RowData) {0};
    }

    RowData aux = rnl->itActual->data;
    rnl->itActual = rnl->itActual->next;
    return aux;
}

int hasNext(RowNodeListADT rnl){
    return rnl != NULL && rnl->itActual != NULL;
}