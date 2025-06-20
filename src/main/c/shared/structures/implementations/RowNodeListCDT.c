#include "../../Type.h"
#include "../RowNodeListADT.h"
#include <stdlib.h>

static int STITCHES_CHUNK_SIZE = 10;

typedef struct RowNode {
    RowData data;
    struct RowNode * next;
} RowNode;

typedef struct RowNodeListCDT {
    RowNode * head;
    RowNode * last;
    RowNode * itActual;
    int size;
} RowNodeListCDT;

RowNodeListADT newRowNodeList(void){
    RowNodeListADT ans = calloc(1,sizeof(RowNodeListCDT));
    return ans;
}

void createRowNode(RowNodeListADT rnl, RowData data){
    if(rnl == NULL) {
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
    rnl->size++;
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

void addStitchToLastnode(RowNodeListADT rnl, StitchType data) {
    if (rnl == NULL || rnl->last == NULL) {
        return;
    }
    RowNode * lastNode = rnl->last;
    if (lastNode->data.stitches == NULL || lastNode->data.stitchCount == 0) {
        lastNode->data.stitches = malloc(STITCHES_CHUNK_SIZE * sizeof(StitchType));
        lastNode->data.stitchCount = 0;
    } else if (lastNode->data.stitchCount % STITCHES_CHUNK_SIZE == 0) {
        lastNode->data.stitches = realloc(lastNode->data.stitches, sizeof(StitchType) * (lastNode->data.stitchCount + STITCHES_CHUNK_SIZE));
    }
    lastNode->data.stitches[lastNode->data.stitchCount++] = data;
}

int getSize(RowNodeListADT rnl) {
    if (rnl == NULL) {
        return 0;
    }
    return rnl->size;
}

void changeColorToLastNode(RowNodeListADT rnl, char* color) {
    if (rnl == NULL || rnl->last == NULL) {
        return;
    }
    RowNode * lastNode = rnl->last;
    if (color != NULL) {
        strncpy(lastNode->data.color, color, 7);
        lastNode->data.color[6] = '\0';
    } else {
        lastNode->data.color[0] = '\0';
    }
}