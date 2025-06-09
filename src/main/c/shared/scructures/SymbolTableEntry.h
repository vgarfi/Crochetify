#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include "../Type.h"
#include "RowNodeListADT.h"

typedef struct {
    VariableType type;

    union {
        StitchType stitchType;
        char color[7];
            // RowNodeListADT * head;
    } data;
} SymbolTableEntry;

#endif