#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../Type.h"
#include "RowNodeListADT.h"
#include "KHash.h"

// typedef khash_t(symbol_table) * SymbolTablePtr;
typedef void* SymbolTablePtr;

typedef struct {
    VariableType paramType;
    void * paramValue;
} PatternParam;

typedef struct {
    int paramCount;
    PatternParam* params;
    RowNodeListADT rowNodeList;
    boolean startsWithTurn;
} PatternData;

typedef struct {
    VariableType type;
    union {
        StitchType stitchType;
        char color[8];
        PatternData PatternData;
    } data;
} SymbolTableEntry;

SymbolTablePtr initSymbolTable();

void putStitchToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, StitchType* stitchType);
void putColorToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, char* color);
void putRowNodeListToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, RowNodeListADT rowList);
void putPatternDataToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, PatternData* PatternData);

StitchType* getStitchFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);
char* getColorFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);
RowNodeListADT getRowNodeListFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);
PatternData* getPatternDataFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);

boolean existsSymbolTableEntry(SymbolTablePtr symbolTablePtr, char* key, VariableType type);
boolean existsSymbolTableEntryWithSameType(SymbolTablePtr symbolTablePtr, char * key, VariableType type);

void freeSymbolTable(SymbolTablePtr symbolTablePtr);
#endif