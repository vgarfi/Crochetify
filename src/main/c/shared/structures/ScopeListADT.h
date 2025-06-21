#ifndef SCOPE_LIST_ADT_H
#define SCOPE_LIST_ADT_H

#include "SymbolTable.h"

typedef struct ScopeListCDT * ScopeListADT;

ScopeListADT newScopeList(void);
void freeScopeList(ScopeListADT list);

void insertNewScope(ScopeListADT list);
void putSymbolInSymbolTable(ScopeListADT list, char* identifier, VariableType type, void* value);
void* getValueByIdentifier(ScopeListADT list, char * identifier, VariableType type);
void removeLastScope(ScopeListADT list);
boolean existsValueByIdentifier(ScopeListADT list, char * identifier, VariableType type);
boolean existsValueByIdentifierAndType(ScopeListADT list, char * identifier, VariableType type);
#endif