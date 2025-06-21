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
    rnl->size++;
}

void freeRowNodeList(RowNodeListADT rnl) {
    if (rnl == NULL) return;

    RowNode *current = rnl->head;
    while (current != NULL) {
        RowNode *aux = current->next;
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
    if (rnl == NULL || rnl->last == NULL) return;
    RowNode *lastNode = rnl->last;
    if (lastNode->data.stitches == NULL || lastNode->data.stitchCount == 0) {
        lastNode->data.stitches = malloc(STITCHES_CHUNK_SIZE * sizeof(StitchType));
        lastNode->data.stitchCount = 0;
    } else if (lastNode->data.stitchCount % STITCHES_CHUNK_SIZE == 0) {
        lastNode->data.stitches = realloc(
            lastNode->data.stitches,
            sizeof(StitchType) * (lastNode->data.stitchCount + STITCHES_CHUNK_SIZE)
        );
    }
    lastNode->data.stitches[lastNode->data.stitchCount++] = data;
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
    beginIteration(sourceRnl);
    while (hasNext(sourceRnl)) {
        createRowNode(destRnl, next(sourceRnl));
    }
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
        // ... agrega aquí el resto de tus puntadas
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
            printf("(TURN) ");
        }

        for (int i = 0; i < rd.stitchCount; ++i) {
            printf("%s", stitchTypeToString(rd.stitches[i]));
            if (i + 1 < rd.stitchCount) printf(" ");
        }

        printf("\n");
    }
}
