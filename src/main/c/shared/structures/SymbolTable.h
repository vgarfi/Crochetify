#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../Type.h"
#include "RowNodeListADT.h"
#include "KHash.h"

// typedef khash_t(symbol_table) * SymbolTablePtr;
typedef void* SymbolTablePtr;

typedef struct {
    VariableType type;
    union {
        StitchType stitchType;
        char color[7];
        RowNodeListADT * rowList;
    } data;
} SymbolTableEntry;

void symbolTablePutEntry(SymbolTablePtr symbolTablePtr,char* key,SymbolTableEntry* value);
SymbolTableEntry* symbolTableGetEntry(SymbolTablePtr symbolTablePtr,char* key);
boolean symbolTableEntryExists(SymbolTablePtr symbolTablePtr,char* key);
SymbolTablePtr initSymbolTable();
void freeSymbolTable(SymbolTablePtr symbolTablePtr);
SymbolTableEntry* newSymbolTableEntry();
#endif