#include <stdlib.h>
#include "../ScopeListADT.h"
#include "../SymbolTable.h"
#include "../KHash.h"
#include <stdio.h>
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

boolean existsValueByIdentifier(ScopeListADT list, char * identifier, VariableType type){
    for(ScopeNode* current = list->head; current != NULL; current = current->next) {
        if(existsSymbolTableEntry(current->symbolTable,identifier,type)){
            return true;
        }
    }
    return false;
}

boolean existsValueByIdentifierAndType(ScopeListADT list, char * identifier, VariableType type){
    for(ScopeNode* current = list->head; current != NULL; current = current->next) {
        if(existsSymbolTableEntryWithSameType(current->symbolTable,identifier,type)){
            return true;
        }
    }
    return false;
}

void* getValueByIdentifier(ScopeListADT list, char * identifier, VariableType type) {
    for(ScopeNode* current = list->head; current != NULL; current = current->next) {
        switch (type) {
        case COLOR_TYPE:
            char* color = getColorFromSymbolTable(current->symbolTable, identifier);
            if (color != NULL) {
                return color;
            }
            break;
        
        case STITCH_TYPE:
            StitchType* stitch = getStitchFromSymbolTable(current->symbolTable, identifier);
            if (stitch != NULL) {
                return stitch;
            }
            break;
        
        case PATTERN_TYPE:
            PatternData* pattern = getPatternDataFromSymbolTable(current->symbolTable, identifier);
            if (pattern != NULL) {
                return pattern;
            }
            break;
        
        default:
            break;
        }
    }
    return NULL;
}

void insertNewScope(ScopeListADT list) {
    ScopeNode * scopeNode = malloc(sizeof(ScopeNode));
    scopeNode->symbolTable = initSymbolTable();
    scopeNode->next = list->head;
    list->head = scopeNode;
}

// Pasar como &value el &patternData
void putSymbolInSymbolTable(ScopeListADT list, char* identifier, VariableType type, void* value){
    switch (type) {
        case COLOR_TYPE:
            putColorToSymbolTable(list->head->symbolTable, identifier, (char*) value);
            break;
        
        case STITCH_TYPE:
            putStitchToSymbolTable(list->head->symbolTable, identifier, (StitchType*) value);
            break;
        
        case PATTERN_TYPE:
            putPatternDataToSymbolTable(list->head->symbolTable, identifier, (PatternData*) value);
            break;
        
        default:
            return;
            break;
    }
}

void removeLastScope(ScopeListADT list) {
    if (!list->head) return;
    ScopeNode * scopeNode = list->head;
    freeSymbolTable(scopeNode->symbolTable);
    list->head = scopeNode->next;
    free(scopeNode);
}