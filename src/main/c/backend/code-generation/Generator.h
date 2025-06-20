#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#define BASE_CANVAS_SIZE 100
#define STITCH_WIDTH 35
#define STITCH_HEIGHT 30

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../domain-specific/Crochet.h"
#include "../../shared/structures/RowNodeListADT.h"
#include <stdarg.h>
#include <stdio.h>

/** Initialize module's internal state. */
void initializeGeneratorModule();

/** Shutdown module's internal state. */
void shutdownGeneratorModule();

/**
 * Generates the final output using the current compiler state.
 */
void generate(CrochetResult * crochetResult);

#endif
