#ifndef CROCHET_HEADER
#define CROCHET_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <limits.h>
#include "../../shared/structures/ScopeListADT.h"
#include "../../shared/structures/RowNodeListADT.h"
#include <stdarg.h>
#include <string.h>
/** Initialize module's internal state. */
void initializeCrochetModule();

/** Shutdown module's internal state. */
void shutdownCrochetModule();

/**
 * The result of a computation. It's considered valid only if "succeed" is
 * true.
 */
typedef struct {
	boolean succeed;
	char * errorMsg;
	RowNodeListADT stitchRows;
} CrochetResult;


/**
 * Computes the final value of a crochet pattern.
 */

 
 
/**
 * Processes the main crochet program and returns the final result.
 * @param program AST of the crochet program.
 * @param scopeList Global symbol table.
 * @return CrochetResult with success status and generated rows.
 */
CrochetResult computeCrochet(Program * program, ScopeListADT scopeList);

/**
 * Processes a single crochet row.
 * @param row Row node.
 * @param scopeList Symbol table.
 * @param rowNodeList List of rows to append results to.
 * @return CrochetResult with success status and the processed row.
 */
CrochetResult computeRow(Row * row, ScopeListADT scopeList, RowNodeListADT rowNodeList);

/**
 * Processes a pattern use (PatternUse).
 * @param patternUse Pattern use node.
 * @param scopeList Symbol table.
 * @param rowNodeList List of rows to append results to.
 * @return CrochetResult with success status and generated rows.
 */
CrochetResult computePatternUse(PatternUse * patternUse, ScopeListADT scopeList, RowNodeListADT rowNodeList);

/**
 * Processes a single argument.
 * @param argument Argument node.
 * @param scopeList Symbol table.
 * @return CrochetResult with success status and the computed value.
 */
CrochetResult computeArgument(Argument * argument, ScopeListADT scopeList);

/**
 * Processes a mirror instruction (Mirror).
 * @param mirror Mirror node.
 * @param scopeList Symbol table.
 * @param rowNodeList List of rows to append results to.
 * @return CrochetResult with success status and generated rows.
 */
CrochetResult computeMirror(Mirror* mirror, ScopeListADT scopeList, RowNodeListADT rowNodeList);

/**
 * Processes a repeat instruction (Repeat).
 * @param repeat Repeat node.
 * @param scopeList Symbol table.
 * @param rowNodeList List of rows to append results to.
 * @return CrochetResult with success status and generated rows.
 */
CrochetResult computeRepeat(Repeat* repeat, ScopeListADT scopeList, RowNodeListADT rowNodeList);

/**
 * Processes a complete pattern.
 * @param patternData Pattern data (parameters, etc).
 * @param pattern Pattern node.
 * @param scopeList Symbol table.
 * @param rowNodeList List of rows to append results to.
 * @return CrochetResult with success status and generated rows.
 */
CrochetResult computePattern(PatternData patternData, Pattern * pattern, ScopeListADT scopeList, RowNodeListADT rowNodeList);

/**
 * Processes a single stitch.
 * @param stitch Stitch node.
 * @param rowNodeList List of rows to append the stitch to.
 * @return CrochetResult with success status and the updated row.
 */
CrochetResult computeStitch(Stitch * stitch, RowNodeListADT rowNodeList);

/**
 * Processes a sequence of stitches (Sequence).
 * @param sequence Sequence node.
 * @param rowNodeList List of rows to append the stitches to.
 * @return CrochetResult with success status and generated rows.
 */
CrochetResult computeStitchList(Sequence * sequence, RowNodeListADT rowNodeList);

/**
 * Processes an identifier (color, stitch, etc).
 * @param identifier Name of the identifier.
 * @param scopeList Symbol table.
 * @param rowNodeList List of rows to apply the identifier to.
 * @return CrochetResult with success status and the updated row.
 */
CrochetResult computeIdentifier(char * identifier, ScopeListADT scopeList, RowNodeListADT rowNodeList);
#endif
