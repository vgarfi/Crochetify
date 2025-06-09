#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../Type.h"
#include "RowNodeListADT.h"
#include "KHash.h"

typedef struct {
    VariableType type;
    union {
        StitchType stitchType;
        char color[7];
        RowNodeListADT * head;
    } data;
} SymbolTableEntry;

void symbolTablePutEntry(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr,char* key,SymbolTableEntry* value);
SymbolTableEntry* symbolTableGetEntry(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr,char* key);
boolean symbolTableEntryExists(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr,char* key);
khash_t(SYMBOL_TABLE_NAME)* initSymbolTable();
void freeSymbolTable(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr);

#endif