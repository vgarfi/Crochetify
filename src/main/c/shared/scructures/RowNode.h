#ifndef ROW_NODE_H
#define ROW_NODE_H

#include <Type.h>

typedef struct {
    char color[6];
    StitchType* stitches;
    RowNode* next;
} RowNode;

#endif