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

/**
 * Initializes a new symbol table.
 * @return A pointer to the created symbol table.
 */
SymbolTablePtr initSymbolTable();

/**
 * Inserts a stitch into the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the stitch variable.
 * @param stitchType Pointer to the stitch type.
 */
void putStitchToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, StitchType* stitchType);

/**
 * Inserts a color into the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the color variable.
 * @param color Color value as a string.
 */
void putColorToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, char* color);

/**
 * Inserts a RowNodeList into the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the variable.
 * @param rowList RowNodeListADT to store.
 */
void putRowNodeListToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, RowNodeListADT rowList);

/**
 * Inserts a PatternData into the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the pattern variable.
 * @param PatternData Pointer to the PatternData.
 */
void putPatternDataToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, PatternData* PatternData);

/**
 * Retrieves a stitch from the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the stitch variable.
 * @return Pointer to the StitchType, or NULL if not found.
 */
StitchType* getStitchFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);

/**
 * Retrieves a color from the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the color variable.
 * @return Pointer to the color string, or NULL if not found.
 */
char* getColorFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);

/**
 * Retrieves a RowNodeList from the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the variable.
 * @return RowNodeListADT, or NULL if not found.
 */
RowNodeListADT getRowNodeListFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);

/**
 * Retrieves a PatternData from the symbol table.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the pattern variable.
 * @return Pointer to the PatternData, or NULL if not found.
 */
PatternData* getPatternDataFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key);

/**
 * Checks if an entry exists in the symbol table with the given type.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the variable.
 * @param type Variable type.
 * @return true if the entry exists, false otherwise.
 */
boolean existsSymbolTableEntry(SymbolTablePtr symbolTablePtr, char* key, VariableType type);

/**
 * Checks if an entry exists in the symbol table with the exact same type.
 * @param symbolTablePtr Pointer to the symbol table.
 * @param key Name of the variable.
 * @param type Variable type.
 * @return true if the entry exists with the same type, false otherwise.
 */
boolean existsSymbolTableEntryWithSameType(SymbolTablePtr symbolTablePtr, char * key, VariableType type);

/**
 * Frees all memory used by the symbol table.
 * @param symbolTablePtr Pointer to the symbol table to free.
 */
void freeSymbolTable(SymbolTablePtr symbolTablePtr);
#endif