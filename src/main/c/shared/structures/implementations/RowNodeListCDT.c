#include "../../Type.h"
#include "../RowNodeListADT.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int STITCHES_CHUNK_SIZE = 10;

typedef struct RowNode {
    RowData data;
    struct RowNode *next;
    struct RowNode *prev;
} RowNode;

typedef struct RowNodeListCDT {
    RowNode *head;
    RowNode *last;
    RowNode *itActual;
    RowNode *itReverseActual;
    int size;
} RowNodeListCDT;

RowNodeListADT newRowNodeList(void) {
    RowNodeListADT ans = calloc(1, sizeof(RowNodeListCDT));
    ////printf("[RowNodeList] newRowNodeList creado: %p\n", (void*)ans);
    return ans;
}

void createRowNode(RowNodeListADT rnl, RowData data) {
    if (rnl == NULL) return;

    RowNode *node = malloc(sizeof(RowNode));
    node->data = data;
    node->next = NULL;

    RowNode *last = rnl->last;
    if (rnl->head == NULL) {
        node->prev = NULL;
        rnl->head = node;
    } else {
        node->prev = last;
        last->next = node;
    }
    rnl->last = node;
    rnl->itActual = NULL;
    rnl->itReverseActual = NULL;
    (rnl->size)++;
    ////printf("[RowNodeList] createRowNode en lista %p, nodo %p\n", (void*)rnl, (void*)node);
}

void freeRowNodeList(RowNodeListADT rnl) {
    if (rnl == NULL) return;

    ////printf("[RowNodeList] freeRowNodeList liberando lista %p\n", (void*)rnl);
    RowNode *current = rnl->head;
    while (current != NULL) {
        ////printf("etoy en whik\n\n");
        RowNode *aux = current->next;
        ////printf("[RowNodeList]   liberando nodo %p (stitches %p)\n", (void*)current, (void*)current->data.stitches);
        free(current->data.stitches);
        free(current);
        current = aux;
    }
    free(rnl);
}

void beginIteration(RowNodeListADT rnl) {
    if (rnl == NULL || rnl->head == NULL) return;
    rnl->itActual = rnl->head;
}

RowData next(RowNodeListADT rnl) {
    if (rnl == NULL || !hasNext(rnl)) return (RowData){0};
    RowData aux = rnl->itActual->data;
    rnl->itActual = rnl->itActual->next;
    return aux;
}

int hasNext(RowNodeListADT rnl) {
    return rnl != NULL && rnl->itActual != NULL;
}

void addStitchToLastnode(RowNodeListADT rnl, StitchType data) {
    if (rnl == NULL || rnl->last == NULL) {
    //    printf("lista : %p  lista last: %p\n",rnl,rnl->last);
//   printf("return \n");
        return;
    }
    RowNode *lastNode = rnl->last;

    if (lastNode->data.stitches == NULL || lastNode->data.stitchCount == 0) {
       // printf(" reset conteo\n");
        lastNode->data.stitches = malloc(STITCHES_CHUNK_SIZE * sizeof(StitchType));
        if (lastNode->data.stitches == NULL) {
          //   printf(" algo salio mal 2\n");
            return;
        }
        lastNode->data.stitchCount = 0;
    } else if (lastNode->data.stitchCount % STITCHES_CHUNK_SIZE == 0) {
        StitchType *newPtr = realloc(
            lastNode->data.stitches,
            sizeof(StitchType) * (lastNode->data.stitchCount + STITCHES_CHUNK_SIZE)
        );
        if (newPtr == NULL) {
          //   printf(" algo salio mal 3\n");
            return;
        }
        lastNode->data.stitches = newPtr;
    }

    lastNode->data.stitches[lastNode->data.stitchCount++] = data;
  //  printf("contador mas uno !Q!!!!!!!!!!!!!!!");
  //  printf("el stitch en posicion %d es de tipo %d\n",lastNode->data.stitchCount-1,lastNode->data.stitches[lastNode->data.stitchCount-1]);
}

int getSize(RowNodeListADT rnl) {
    if (rnl == NULL) return 0;
    return rnl->size;
}

void changeColorToLastNode(RowNodeListADT rnl, char* color) {
    if (rnl == NULL || rnl->last == NULL) return;
    RowNode *lastNode = rnl->last;
    if (color != NULL) {
        strncpy(lastNode->data.color, color, sizeof(lastNode->data.color) - 1);
        lastNode->data.color[sizeof(lastNode->data.color) - 1] = '\0';
    } else {
        lastNode->data.color[0] = '\0';
    }
}

void appendList(RowNodeListADT destRnl, RowNodeListADT sourceRnl) {
    if (destRnl == NULL || sourceRnl == NULL) return;

    ////printf("[RowNodeList] appendList: destino %p, fuente %p\n", (void*)destRnl, (void*)sourceRnl);
    if (sourceRnl->head == NULL) return; // Nothing to append
    if (destRnl->head == NULL) {
        destRnl->head = sourceRnl->head;
        destRnl->last = sourceRnl->last;
    } else {
        destRnl->last->next = sourceRnl->head;
        sourceRnl->head->prev = destRnl->last;
        destRnl->last = sourceRnl->last;
    }
    destRnl->size += sourceRnl->size;
    destRnl->itActual = NULL;
    destRnl->itReverseActual = NULL;
}

void beginReverseIteration(RowNodeListADT rnl) {
    if (rnl == NULL || rnl->last == NULL) {
        if (rnl) rnl->itReverseActual = NULL;
        return;
    }
    rnl->itReverseActual = rnl->last;
}

RowData nextReverse(RowNodeListADT rnl) {
    if (rnl == NULL || !hasNextReverse(rnl)) return (RowData){0};
    RowData aux = rnl->itReverseActual->data;
    rnl->itReverseActual = rnl->itReverseActual->prev;
    return aux;
}

int hasNextReverse(RowNodeListADT rnl) {
    return rnl != NULL && rnl->itReverseActual != NULL;
}

static const char* stitchTypeToString(StitchType st) {
    switch (st) {
        case STITCH_CH: return "CH";
        case STITCH_DC: return "DC";
        case STITCH_SC: return "SC";
        default: return "?";
    }
}

void printRowList(RowNodeListADT rnl) {
    int count = 0;
    if (!rnl) return;
    beginIteration(rnl);
    while (hasNext(rnl)) {
    
        RowData rd = next(rnl);
        if ((count++) % 2 == 1) {
            //printf("TURN");
        }

        for (int i = 0; i < rd.stitchCount; ++i) {
            //printf("%s", stitchTypeToString(rd.stitches[i]));
          //  if (i + 1 < rd.stitchCount) //printf("  ");
        }

    }
}

void deintegrateList(RowNodeListADT rnl) {
    if (rnl == NULL) return;
    rnl->head = NULL;
}

void deintegrateListAndFirst(RowNodeListADT rnl) {
    if (rnl == NULL || rnl->head == NULL)
        return;
    RowNode *first = rnl->head;
    RowNode *next = first->next;
    ////printf("[RowNodeList] deintegrateListAndFirst liberando primer nodo %p (stitches %p)\n", (void*)first, (void*)first->data.stitches);
    if (next != NULL) {
        next->prev = NULL;
    }

    // Liberar el primer nodo y su arreglo de stitches
    if (first->data.stitches != NULL) {
        free(first->data.stitches);
    }
    free(first);
    rnl->head = NULL; // La estructura queda "vacía"
}

void appendNodeandList(RowNodeListADT destRnl, RowNodeListADT sourceRnl){
    if (destRnl == NULL || sourceRnl == NULL) return;
    if (sourceRnl->head == NULL || destRnl->last == NULL) return; // Nada que hacer

    ////printf("[RowNodeList] appendNodeandList: destino %p, fuente %p\n", (void*)destRnl, (void*)sourceRnl);
    RowNode *srcNode = sourceRnl->head;

    for (int i = 0; i < srcNode->data.stitchCount; ++i) {
        addStitchToLastnode(destRnl, srcNode->data.stitches[i]);
    }
    if (srcNode->next != NULL) {
        destRnl->last->next = srcNode->next;
        srcNode->next->prev = destRnl->last;
        destRnl->last = sourceRnl->last;
        destRnl->size += (sourceRnl->size - 1);
    }
    destRnl->itActual = NULL;
    destRnl->itReverseActual = NULL;
}

RowNodeListADT cloneRowNodeList(RowNodeListADT src) {
    if (src == NULL) return NULL;
    RowNodeListADT dst = newRowNodeList();
    //printf("lista :");
    printRowList(src);
    int i = 1;
    ////printf("[RowNodeList] cloneRowNodeList: origen %p, copia %p\n", (void*)src, (void*)dst);
    RowNode *curr = src->head;
    while (curr != NULL) {
        //printf(" elemento %d copiado\n",i);
        RowData newData;
        memcpy(newData.color, curr->data.color, sizeof(newData.color));
        newData.stitchCount = curr->data.stitchCount;
        if (curr->data.stitchCount > 0 && curr->data.stitches != NULL) {
            newData.stitches = malloc(sizeof(StitchType) * curr->data.stitchCount);
            memcpy(newData.stitches, curr->data.stitches, sizeof(StitchType) * curr->data.stitchCount);
        } else {
            newData.stitches = NULL;
        }
        createRowNode(dst, newData);
        curr = curr->next;
    }
    return dst;
}

void deintegrateListFull(RowNodeListADT rnl) {
    if (rnl == NULL) return;
    RowNode *current = rnl->head;
    while (current != NULL) {
        RowNode *aux = current->next;
        free(current->data.stitches);
        free(current);
        current = aux;
    }
    rnl->head = NULL;
    rnl->last = NULL;
}

void printAllRowData(RowNodeListADT rnl) {
    if (!rnl) return;
    int row = 0;
    beginIteration(rnl);
    while (hasNext(rnl)) {
        RowData rd = next(rnl);
        printf("Row %d: color='%s', stitchCount=%d, stitches=[", row, rd.color, rd.stitchCount);
        for (int i = 0; i < rd.stitchCount; ++i) {
            printf("%s", stitchTypeToString(rd.stitches[i]));
            if (i + 1 < rd.stitchCount) printf(", ");
        }
        printf("]\n");
        row++;
    }
}