#include "../SymbolTable.h"

void symbolTablePutEntry(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr,char* key,SymbolTableEntry* value){
    if (!symbol_table_exists(name)) {

        Variable* new = create_new_var(name, type);

        int ret;
        khiter_t k = kh_put(variables_hash_map, variables_hm, name, &ret);
        if (ret == -1) {
            return NULL;
        }

        kh_val(variables_hm, k) = new;
        return new;
    }
    
    return NULL;
}

SymbolTableEntry* symbolTableGetEntry(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr,char* key){
    khiter_t k = kh_get(SYMBOL_TABLE_NAME, symbolTablePtr, name);
    if (k != kh_end(symbolTablePtr)) {
        return kh_val(symbolTablePtr, k);
    }
    return NULL;
}

boolean symbolTableEntryExists(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr,char* key){
    khiter_t k = kh_get(SYMBOL_TABLE_NAME, symbolTablePtr, key);
    return k != kh_end(symbolTablePtr);
}

khash_t(SYMBOL_TABLE_NAME)* initSymbolTable(){
    return kh_init(SYMBOL_TABLE_NAME);
}

void freeSymbolTable(khash_t(SYMBOL_TABLE_NAME)* symbolTablePtr){

}

void freeSymbolTableEntry(SymbolTableEntry* symbolTableEntry){

}