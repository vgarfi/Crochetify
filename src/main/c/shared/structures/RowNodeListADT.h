#ifndef ROW_NODE_LIST_ADT_H
#define ROW_NODE_LIST_ADT_H

#include <string.h>
#include "../Type.h"

typedef struct RowNodeListCDT * RowNodeListADT;

/**
 * Creates a new RowNodeList abstract data type.
 * @return A new RowNodeListADT instance.
 */
RowNodeListADT newRowNodeList(void);

/**
 * Inserts the given row data into the list.
 * This action resets iteration; after any insertion you must use 'beginIteration' to iterate elements.
 * @param rnl Existing RowNodeListADT.
 * @param data RowData to insert into the list.
 */
void createRowNode(RowNodeListADT rnl, RowData data);

/**
 * Frees all resources used by the RowNodeListADT. Makes the ADT unusable.
 * @param rnl Existing RowNodeListADT.
 */
void freeRowNodeList(RowNodeListADT rnl);

/**
 * Prepares the RowNodeListADT for forward iteration.
 * @param rnl Existing RowNodeListADT.
 */
void beginIteration(RowNodeListADT rnl);

/**
 * Prepares the RowNodeListADT for reverse iteration.
 * @param rnl Existing RowNodeListADT.
 */
void beginReverseIteration(RowNodeListADT rnl);

/**
 * Returns the next row data in reverse iteration.
 * @param rnl Existing RowNodeListADT.
 * @return The next RowData in reverse order.
 */
RowData nextReverse(RowNodeListADT rnl);

/**
 * Checks if there is more data to extract in reverse iteration.
 * @param rnl Existing RowNodeListADT.
 * @return 1 if there is more data, 0 otherwise.
 */
int hasNextReverse(RowNodeListADT rnl);

/**
 * Returns the next row data in forward iteration.
 * @param rnl Existing RowNodeListADT.
 * @return The next RowData in forward order.
 */
RowData next(RowNodeListADT rnl);

/**
 * Checks if there is more data to extract in forward iteration.
 * @param rnl Existing RowNodeListADT.
 * @return 1 if there is more data, 0 otherwise.
 */
int hasNext(RowNodeListADT rnl);

/**
 * Inserts the given stitch into the last node of the list.
 * @param rnl Existing RowNodeListADT.
 * @param data StitchType to insert.
 */
void addStitchToLastnode(RowNodeListADT rnl, StitchType data);

/**
 * Appends all nodes from the source RowNodeListADT to the end of the destination RowNodeListADT.
 * @param destRnl Destination RowNodeListADT.
 * @param sourceRnl Source RowNodeListADT.
 */
void appendList(RowNodeListADT destRnl, RowNodeListADT sourceRnl);

/**
 * Appends all nodes from the source RowNodeListADT to the end of the destination RowNodeListADT, except the first node.
 * @param destRnl Destination RowNodeListADT.
 * @param sourceRnl Source RowNodeListADT.
 */
void appendNodeandList(RowNodeListADT destRnl, RowNodeListADT sourceRnl);

/**
 * Changes the color of the last node in the list.
 * @param rnl Existing RowNodeListADT.
 * @param color Color string to insert.
 */
void changeColorToLastNode(RowNodeListADT rnl, char* color);

/**
 * Returns the actual size (number of nodes) of the list.
 * @param rnl Existing RowNodeListADT.
 * @return The size of the list.
 */
int getSize(RowNodeListADT rnl);

/**
 * Prints the contents of the RowNodeListADT.
 * @param rnl Existing RowNodeListADT.
 */
void printRowList(RowNodeListADT rnl);

/**
 * Deintegration utility: detaches or clears the list.
 * @param rnl Existing RowNodeListADT.
 */
void deintegrateList(RowNodeListADT rnl);

/**
 * Deintegration utility: detaches or clears the list except the first node.
 * @param rnl Existing RowNodeListADT.
 */
void deintegrateListAndFirst(RowNodeListADT rnl);

/**
 * Creates a deep copy of the given RowNodeListADT.
 * @param src Source RowNodeListADT.
 * @return A new RowNodeListADT that is a clone of the source.
 */
RowNodeListADT cloneRowNodeList(RowNodeListADT src);

/**
 * Deintegration utility: fully detaches or clears the list.
 * @param rnl Existing RowNodeListADT.
 */
void deintegrateListFull(RowNodeListADT rnl);

/**
 * Prints all RowData elements in the RowNodeListADT.
 * @param rnl Existing RowNodeListADT.
 */
void printAllRowData(RowNodeListADT rnl);

#endif