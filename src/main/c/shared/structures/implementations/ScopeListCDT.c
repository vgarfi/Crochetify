#include <stdlib.h>
#include "../ScopeListADT.h"
#include "../SymbolTable.h"
#include "../KHash.h"

typedef struct ScopeNode {
    SymbolTablePtr symbolTable;
    struct ScopeNode * next;    
} ScopeNode;

struct ScopeListCDT {
    ScopeNode * head;
} ScopeListCDT;

ScopeListADT newScopeList(void) {
    ScopeListADT list = malloc(sizeof(struct ScopeListCDT));
    list->head = NULL;
    return list;
}

void freeScopeList(ScopeListADT list) {
    ScopeNode * curr = list->head;
    while (curr) {
        ScopeNode * tmp = curr;
        curr = curr->next;
        freeSymbolTable(tmp->symbolTable);
        free(tmp);
    }
    free(list);
}

SymbolTableEntry * getIdentifierValue(ScopeListADT list, char * identifier){
    SymbolTableEntry* value = NULL;
    for(ScopeNode* current = list->head; current != NULL && value == NULL; current = current->next) {
        // value = symbolTableGetEntry(current->symbolTable, identifier);
    }
    return value;
}

void insertNewScope(ScopeListADT list) {
    ScopeNode * scopeNode = malloc(sizeof(ScopeNode));
    scopeNode->symbolTable = initSymbolTable();
    scopeNode->next = list->head;
    list->head = scopeNode;
}

void addSymbolTableEntry(ScopeListADT list, char* identifier, VariableType type){
    // SymbolTableEntry * symbolTableEntry = newSymbolTableEntry();
    // symbolTableEntry->type = type;
    // symbolTablePutIfAbsentEntry(list->head->symbolTable, identifier, symbolTableEntry);
}

void putSymbolTableEntry(ScopeListADT list, char* identifier, SymbolTableEntry symbolTableEntry){
    // symbolTablePutEntry(list->head->symbolTable, identifier, &entrada);
}

void removeLastScope(ScopeListADT list) {
    if (!list->head) return;
    ScopeNode * scopeNode = list->head;
    freeSymbolTable(scopeNode->symbolTable);
    list->head = scopeNode->next;
    free(scopeNode);
}