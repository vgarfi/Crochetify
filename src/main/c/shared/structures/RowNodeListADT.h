#ifndef ROW_NODE_LIST_ADT_H
#define ROW_NODE_LIST_ADT_H

typedef struct RowNodeListCDT * RowNodeListADT;

/**
*   @return new ADT to be used in the following functions
*/
RowNodeListADT newRowNodeList(void);

/**
*   @param rnl existing ADT
*   @param data information to insert into list
*
*   Inserts the given data into the list described by the ADT
*   This action resets iteration and implies that after
*   any insertion you must use 'beginIteration' in order to iterate elements.
*/
void createRowNode(RowNodeListADT rnl, RowData data);

/**
*   @param rnl existing ADT
*
*   Clears the resources used for the ADT. Makes the ADT unusable.
*/
void freeRowNodeList(RowNodeListADT rnl);

/*
*   @param rnl existing ADT
*
*   Allows the ADT to be iterated 
*/
void beginIteration(RowNodeListADT rnl);

/**
*   @param rnl existing ADT
*
*   @return the next row data
 */
RowData next(RowNodeListADT rnl);

/**
*   @param rnl existing ADT
*
*   @return 1 if there is more data to extract, 0 if not.
*/
int hasNext(RowNodeListADT rnl);

#endif