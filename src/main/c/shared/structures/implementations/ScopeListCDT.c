#include <stdlib.h>
#include "../ScopeListADT.h"
#include "../SymbolTable.h"
#include "../KHash.h"

// Mapa <String, SymbolTableEntry*>
KHASH_MAP_INIT_STR(SYMBOL_TABLE_NAME, SymbolTableEntry*)

/*
nt ret, is_missing;
	khiter_t k;
	khash_t(32) *h = kh_init(32);
	k = kh_put(32, h, 5, &ret);
	kh_value(h, k) = 10;
	k = kh_get(32, h, 10);

*/

typedef struct ScopeNode {
    khash_t(SYMBOL_TABLE_NAME)* symbolTable;
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

void free(ScopeListADT list) {
    ScopeNode * curr = list->head;
    while (curr) {
        ScopeNode * tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(list);
}

SymbolTableEntry * getIdentifierValue(char * identifier, khash_t(SYMBOL_TABLE_NAME)* symbolTablePointer){
    SymbolTableEntry* value = NULL;
    khiter_t k = kh_get(symbolTable, symbolTablePointer, identifier);
    
    if (k != kh_end(symbolTablePointer)) {
        value = kh_value(symbolTablePointer, k);
    }
    return value;
}

void insertNewScope(ScopeListADT list, void * data) {
    ScopeNode * scopeNode = malloc(sizeof(ScopeNode));
    scopeNode->symbolTable = initSymbolTable();
    scopeNode->next = list->head;
    list->head = scopeNode;
}

void removeLastScope(ScopeListADT list) {
    if (!list->head) return NULL;
    ScopeNode * scopeNode = list->head;
    // if (scopeNode->symbolTable) {
    //     kh_clear(symbolTable, scopeNode->symbolTable);
    // }
    // kh_destroy(symbolTable, scopeNode->symbolTable);
    freeSymbolTable(scopeNode->symbolTable);
    list->head = scopeNode->next;
    free(scopeNode);
}