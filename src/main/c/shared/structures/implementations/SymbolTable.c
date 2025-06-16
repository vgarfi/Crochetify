#include "../KHash.h"
#include "../RowNodeListADT.h"
#include "../SymbolTable.h"
#include "../../Type.h"
#include <stdio.h>

static void freeSymbolTableEntry(SymbolTableEntry* symbolTableEntryPtr);

KHASH_MAP_INIT_STR(symbol_table, SymbolTableEntry*)

static SymbolTableEntry* newSymbolTableEntry() {
    SymbolTableEntry* entry = calloc(1, sizeof(SymbolTableEntry));
    return entry;
}

static void putEntry(SymbolTablePtr symbolTablePtr, char* compositeKey, SymbolTableEntry* entry) {
    int ret;
    khiter_t k = kh_put(symbol_table, (khash_t(symbol_table)*)symbolTablePtr, compositeKey, &ret);
    SymbolTableEntry* old = NULL;
    if (ret == 0) { // key already existed
        old = kh_val((khash_t(symbol_table)*)symbolTablePtr, k);
        freeSymbolTableEntry(old);
        free(compositeKey); // free the new key, keep the old one
    } else if (ret == 1) {
        kh_key((khash_t(symbol_table)*)symbolTablePtr, k) = compositeKey; // assign only if new
    }
    kh_val((khash_t(symbol_table)*)symbolTablePtr, k) = entry;
}

static char* makeCompositeKey(const char* key, VariableType type) {
    size_t len = strlen(key) + 1 + 12; // 1 for '_' and up to 11 for int + '\0'
    char* composite = malloc(len);
    snprintf(composite, len, "%s_%d", key, (int)type);
    return composite;
}

static SymbolTableEntry* getEntry(SymbolTablePtr symbolTablePtr, const char* key, VariableType type) {
    char* compositeKey = makeCompositeKey(key, type);
    khiter_t k = kh_get(symbol_table, (khash_t(symbol_table)*)symbolTablePtr, compositeKey);
    free(compositeKey); // compositeKey is not stored, just used for lookup
    if (k != kh_end((khash_t(symbol_table)*)symbolTablePtr)) {
        return kh_val((khash_t(symbol_table)*)symbolTablePtr, k);
    }
    return NULL;
}


static void* deepCopyParamValue(VariableType type, void* value) {
   
    switch (type) {
        case COLOR_TYPE: 
            if (!value) return NULL;
            return strdup((char*)value);

        case STITCH_TYPE:
            if (!value) return NULL;
            // StitchType is an enum, passed as pointer to int
            StitchType* copy = malloc(sizeof(StitchType));
            *copy = *(StitchType*)value;
            return copy;
        case PATTERN_TYPE:
            if (!value) {
                value = newRowNodeList();
                // la otra opcion es esto devuelva null
            }
            return value;
        default:
            return NULL;
    }
}

void putStitchToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, StitchType* stitchType) {
    SymbolTableEntry* entry = newSymbolTableEntry();
    entry->type = STITCH_TYPE;
    if(stitchType != NULL){
        entry->data.stitchType = *stitchType;
    }
   else{
     entry->data.stitchType = STITCH_INVALID;
   }
    char* compositeKey = makeCompositeKey(key, STITCH_TYPE);
    putEntry(symbolTablePtr, compositeKey, entry);
}

void putColorToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, char* color) {
    SymbolTableEntry* entry = newSymbolTableEntry();
    entry->type = COLOR_TYPE;
    if(color != NULL){
        strncpy(entry->data.color, color, sizeof(entry->data.color) - 1);
        entry->data.color[sizeof(entry->data.color) - 1] = '\0';
    }
    char* compositeKey = makeCompositeKey(key, COLOR_TYPE);
    putEntry(symbolTablePtr, compositeKey, entry);
}

void putRowNodeListToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, RowNodeListADT rowList) {
    SymbolTableEntry* entry = newSymbolTableEntry();
    entry->type = PATTERN_TYPE;
    entry->data.PatternData.rowNodeList = rowList;
    char* compositeKey = makeCompositeKey(key, PATTERN_TYPE);
    putEntry(symbolTablePtr, compositeKey, entry);
}

void putPatternDataToSymbolTable(SymbolTablePtr symbolTablePtr, char* key, PatternData* patternData) {
    SymbolTableEntry* entry = newSymbolTableEntry();
    if(entry == NULL){
        return;
    }
    entry->type = PATTERN_TYPE;
    if (patternData == NULL) {
        char* compositeKey = makeCompositeKey(key, PATTERN_TYPE);
        putEntry(symbolTablePtr, compositeKey, entry);
        return;
    }
    PatternData src = *patternData;
    PatternData* dest = &entry->data.PatternData;
    dest->paramCount = src.paramCount;

    if (src.paramCount > 0 && src.params != NULL) {
        dest->params = malloc(src.paramCount * sizeof(PatternParam));
        for (int i = 0; i < src.paramCount; i++) {
            dest->params[i].paramType = src.params[i].paramType;
            dest->params[i].paramValue = deepCopyParamValue(
                src.params[i].paramType,
                src.params[i].paramValue
            );
        }
    } else {
        dest->params = NULL;
    }

    if (src.rowNodeList) {
        dest->rowNodeList = src.rowNodeList;
    } else {
        dest->rowNodeList = newRowNodeList();
    }
    char* compositeKey = makeCompositeKey(key, PATTERN_TYPE);
    putEntry(symbolTablePtr, compositeKey, entry);
}

StitchType* getStitchFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key) {
    SymbolTableEntry* entry = getEntry(symbolTablePtr, key, STITCH_TYPE);
    if (entry && entry->type == STITCH_TYPE && entry->data.stitchType != STITCH_INVALID) {
        return &entry->data.stitchType;
    }
    return NULL;
}

char* getColorFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key) {
    SymbolTableEntry* entry = getEntry(symbolTablePtr, key, COLOR_TYPE);
    if (entry && entry->type == COLOR_TYPE) {
        return entry->data.color;
    }
    return NULL;
}

RowNodeListADT getRowNodeListFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key) {
    SymbolTableEntry* entry = getEntry(symbolTablePtr, key, PATTERN_TYPE);
    if (entry && entry->type == PATTERN_TYPE) {
        return entry->data.PatternData.rowNodeList;
    }
    return NULL;
}

PatternData* getPatternDataFromSymbolTable(SymbolTablePtr symbolTablePtr, char* key) {
    SymbolTableEntry* entry = getEntry(symbolTablePtr, key, PATTERN_TYPE);
    if (entry && entry->type == PATTERN_TYPE) {
        return &entry->data.PatternData;
    }
    return NULL;
}

SymbolTablePtr initSymbolTable(){
    return kh_init(symbol_table);
}

void freeSymbolTable(SymbolTablePtr symbolTablePtr) {
    SymbolTableEntry* symbolTableEntryPtr;
    kh_foreach_value((khash_t(symbol_table) *)symbolTablePtr, symbolTableEntryPtr, {
        freeSymbolTableEntry(symbolTableEntryPtr);
    });

    khash_t(symbol_table)* ht = (khash_t(symbol_table)*)symbolTablePtr; 
    for (khiter_t k = kh_begin(ht); k != kh_end(ht); ++k) {
        if (kh_exist(ht, k)) {
            char* key = (char*)kh_key(ht, k);
            free(key);
        }
    }
    kh_destroy(symbol_table, (khash_t(symbol_table) *)symbolTablePtr);
}

boolean existsSymbolTableEntry(SymbolTablePtr symbolTablePtr, char* key, VariableType type) {
    char* compositeKey = makeCompositeKey(key, type);
    khiter_t k = kh_get(symbol_table, (khash_t(symbol_table)*)symbolTablePtr, compositeKey);
    free(compositeKey);
    return k != kh_end((khash_t(symbol_table)*)symbolTablePtr);
}

static void freeSymbolTableEntry(SymbolTableEntry* symbolTableEntryPtr) {
    if (!symbolTableEntryPtr) return;

    if (symbolTableEntryPtr->type == PATTERN_TYPE) {
        PatternData* pd = &symbolTableEntryPtr->data.PatternData;
        if (pd->params != NULL) {
            for (int i = 0; i < pd->paramCount; ++i) {
                if (pd->params[i].paramValue) {
                    free(pd->params[i].paramValue);
                }
            }
            free(pd->params);
        }
        if (pd->rowNodeList != NULL) {
            freeRowNodeList(pd->rowNodeList);
        }
    }
    free(symbolTableEntryPtr);
}

