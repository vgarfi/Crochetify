#ifndef SCOPE_LIST_ADT_H
#define SCOPE_LIST_ADT_H

#include <SymbolTableEntry.h>

typedef struct ScopeListCDT * ScopeListADT;

ScopeListADT newScopeList(void);
void free(ScopeListADT list);

void insertNewScope(ScopeListADT list, void * data);
SymbolTableEntry * getIdentifierValue(char * identifier);
void removeLastScope(ScopeListADT list);

#endif