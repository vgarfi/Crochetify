#include <stdlib.h>
#include "../ScopeListADT.h"
#include "../SymbolTableEntry.h"
#include "../KHash.h"

KHASH_MAP_INIT_INT(32, char)
/*
int ret, is_missing;
	khiter_t k;
	khash_t(32) *h = kh_init(32);
	k = kh_put(32, h, 5, &ret);
	kh_value(h, k) = 10;
	k = kh_get(32, h, 10);

*/
typedef struct ScopeNode {
   // khash_t(32) symbolTable;

    struct ScopeNode * next;
} ScopeNode;

struct ScopeListCDT {
    ScopeNode * head;
} ScopeListCDT;



ScopeListADT newScopeList(void) {
    ScopeListADT list = malloc(sizeof(struct ScopeListCDT));
    list->head = NULL;
    khash_t(32) *h = kh_init(32);

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

SymbolTableEntry * getIdentifierValue(char * identifier){
    
}

void insertNewScope(ScopeListADT list, void * data) {
    ScopeNode * ScopeNode = malloc(sizeof(ScopeNode));
    // ScopeNode->data = data;
    ScopeNode->next = list->head;
    list->head = ScopeNode;
}

void removeLastScope(ScopeListADT list) {
    if (!list->head) return NULL;
    ScopeNode * ScopeNode = list->head;
    // void * data = ScopeNode->data;
    list->head = ScopeNode->next;
    free(ScopeNode);
}

