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
	RowNodeListADT stitchRows;
} CrochetResult;


/**
 * Computes the final value of a crochet pattern.
 */
CrochetResult computeCrochet(Program * program, ScopeListADT scopeList);

CrochetResult computeRow(Row * row, ScopeListADT scopeList,RowNodeListADT rowNodeList);

CrochetResult computePatternUse(PatternUse * patternUse,ScopeListADT scopeList,RowNodeListADT rowNodeList);

CrochetResult computeArgument(Argument * argument,ScopeListADT scopeList);

CrochetResult computeMirror(Mirror* mirror, ScopeListADT scopeList, RowNodeListADT rowNodeList);

CrochetResult computeRepeat(Repeat* repeat, ScopeListADT scopeList, RowNodeListADT rowNodeList);

CrochetResult computePattern(PatternData patternData, Pattern * pattern, ScopeListADT scopeList, RowNodeListADT rowNodeList);

CrochetResult computeStitch(Stitch * stitch, RowNodeListADT rowNodeList);

CrochetResult computeStitchList(Sequence * sequence, RowNodeListADT rowNodeList);

CrochetResult computeIdentifier(char * identifier, ScopeListADT scopeList);

#endif
