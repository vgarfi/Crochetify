#ifndef SCOPE_LIST_ADT_H
#define SCOPE_LIST_ADT_H

#include "SymbolTable.h"

typedef struct ScopeListCDT * ScopeListADT;

/**
 * Creates a new scope list (stack of symbol tables).
 * @return A new ScopeListADT instance.
 */
ScopeListADT newScopeList(void);

/**
 * Frees all memory used by the scope list.
 * @param list The scope list to free.
 */
void freeScopeList(ScopeListADT list);

/**
 * Inserts a new (empty) scope on top of the scope list.
 * @param list The scope list.
 */
void insertNewScope(ScopeListADT list);

/**
 * Inserts a symbol into the symbol table at the top of the scope list.
 * @param list The scope list.
 * @param identifier The name of the symbol.
 * @param type The variable type.
 * @param value The value to store.
 */
void putSymbolInSymbolTable(ScopeListADT list, char* identifier, VariableType type, void* value);

/**
 * Retrieves a value by identifier and type, searching from the top scope down.
 * @param list The scope list.
 * @param identifier The name of the symbol.
 * @param type The variable type.
 * @return Pointer to the value, or NULL if not found.
 */
void* getValueByIdentifier(ScopeListADT list, char * identifier, VariableType type);

/**
 * Removes the last (top) scope from the scope list.
 * @param list The scope list.
 */
void removeLastScope(ScopeListADT list);

/**
 * Checks if a value with the given identifier and type exists in any scope.
 * @param list The scope list.
 * @param identifier The name of the symbol.
 * @param type The variable type.
 * @return true if the value exists, false otherwise.
 */
boolean existsValueByIdentifier(ScopeListADT list, char * identifier, VariableType type);

/**
 * Checks if a value with the given identifier and type exists in the current (top) scope.
 * @param list The scope list.
 * @param identifier The name of the symbol.
 * @param type The variable type.
 * @return true if the value exists in the current scope, false otherwise.
 */
boolean existsValueByIdentifierAndType(ScopeListADT list, char * identifier, VariableType type);

#endif