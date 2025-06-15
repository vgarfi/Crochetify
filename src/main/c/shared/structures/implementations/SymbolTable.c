#include "../KHash.h"
#include "../SymbolTable.h"
#include "../../Type.h"


KHASH_MAP_INIT_STR(symbol_table, SymbolTableEntry*)

SymbolTableEntry* newSymbolTableEntry();
void freeSymbolTableEntry(SymbolTableEntry* symbolTableEntryPtr);

// TODO: implementar las acciones de la symbol table y renombrar aquellas viejas

// void symbolTablePutEntry(SymbolTablePtr symbolTablePtr, char* key, SymbolTableEntry* value){
//     if (symbolTableEntryExists(value,key))
//         return;
    
//     int ret;
//     khiter_t k = kh_put(symbol_table,(khash_t(symbol_table) *) symbolTablePtr, key, &ret);
    
//     if (ret == -1) {
//         return;
//     }

//     kh_val((khash_t(symbol_table) *)symbolTablePtr, k) = value;
// }



// void symbolTablePutIfAbsentEntry(SymbolTablePtr symbolTablePtr, char* key, SymbolTableEntry* value){
//     if (symbolTableEntryExists(value,key))
//         return;
//     symbolTablePutEntry(symbolTablePtr,key,value);
// }

// SymbolTableEntry* symbolTableGetEntry(SymbolTablePtr symbolTablePtr,char* key){
//     khiter_t k = kh_get(symbol_table, (khash_t(symbol_table) *)symbolTablePtr, key);
//     if (k != kh_end((khash_t(symbol_table) *)symbolTablePtr)) {
//         return kh_val((khash_t(symbol_table) *)symbolTablePtr, k);
//     }
//     return NULL;
// }

// boolean symbolTableEntryExists(SymbolTablePtr symbolTablePtr,char* key){
//     khiter_t k = kh_get(symbol_table,(khash_t(symbol_table) *) symbolTablePtr, key);
//     return k != kh_end((khash_t(symbol_table) *)symbolTablePtr);
// }

SymbolTablePtr initSymbolTable(){
    return kh_init(symbol_table);
}

void freeSymbolTable(SymbolTablePtr symbolTablePtr) {
    SymbolTableEntry* symbolTableEntryPtr;
    kh_foreach_value((khash_t(symbol_table) *)symbolTablePtr, symbolTableEntryPtr, {freeSymbolTableEntry(symbolTableEntryPtr);});
    kh_destroy(symbol_table, (khash_t(symbol_table) *)symbolTablePtr);
}

void freeSymbolTableEntry(SymbolTableEntry* symbolTableEntryPtr) {
    
}

// SymbolTableEntry* newSymbolTableEntry() {
//     SymbolTableEntry* symbolTableEntry = calloc(1,sizeof(SymbolTableEntry));
//     return symbolTableEntry;
// }