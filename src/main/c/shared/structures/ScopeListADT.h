#ifndef SCOPE_LIST_ADT_H
#define SCOPE_LIST_ADT_H

#include "SymbolTable.h"

typedef struct ScopeListCDT * ScopeListADT;

ScopeListADT newScopeList(void);
void freeScopeList(ScopeListADT list);

void insertNewScope(ScopeListADT list);
void putSymbolTableEntry(ScopeListADT list, char* identifier, SymbolTableEntry symbolTableEntry);
SymbolTableEntry * getIdentifierValue(ScopeListADT list, char * identifier);
void addSymbolTableEntry(ScopeListADT list, char* identifier, VariableType type);
void removeLastScope(ScopeListADT list);
#endif