#include "AbstractSyntaxTree.h"

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
    _logger = createLogger("AbstractSyntaxTree");
}

void shutdownAbstractSyntaxTreeModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

// Destructor helpers
static void releaseStitch(Stitch *stitch) {
    if (stitch) free(stitch);
}

static void releaseRow(Row *row);
static void releaseTurn(Turn *turn) {
    if (turn) {
        if (turn->color) free(turn->color);
        free(turn);
    }
}
static void releaseRepeat(Repeat *repeat);
static void releaseMirror(Mirror *mirror);
static void releasePattern(Pattern *pattern);
static void releasePatternUse(PatternUse *patternUse) {
    if (patternUse) {
        if (patternUse->name) free(patternUse->name);
        free(patternUse);
    }
}
static void releaseColorRow(ColorRow *colorRow);
static void releaseSequence(Sequence *seq);

static void releaseRow(Row *row) {
    if (row) {
        releaseSequence(row->elements);
        free(row);
    }
}

static void releaseRepeat(Repeat *repeat) {
    if (repeat) {
        releaseSequence(repeat->pattern);
        if (repeat->extra) releaseStitch(repeat->extra);
        free(repeat);
    }
}

static void releaseMirror(Mirror *mirror) {
    if (mirror) {
        releaseSequence(mirror->pattern);
        free(mirror);
    }
}

static void releasePattern(Pattern *pattern) {
    if (pattern) {
        if (pattern->name) free(pattern->name);
        releaseSequence(pattern->body);
        free(pattern);
    }
}

static void releaseColorRow(ColorRow *colorRow) {
    if (colorRow) {
        if (colorRow->color) free(colorRow->color);
        if (colorRow->row) releaseRow(colorRow->row);
        free(colorRow);
    }
}

static void releaseSequence(Sequence *seq) {
    if (seq) {
        for (int i = 0; i < seq->count; ++i) {
            void *item = seq->items[i];
            // Aquí deberías tener un mecanismo para saber el tipo real de cada item.
            // Por simplicidad, asume que todos son Row* (ajusta según tu implementación real).
            // Por ejemplo, podrías tener un array paralelo de enums con el tipo de cada item.
            // releaseRow((Row*)item);
            // --- ADAPTA ESTO SEGÚN TU DISEÑO ---
            // Ejemplo: si tienes un enum* seq->types;
            // switch(seq->types[i]) { ... }
        }
        free(seq->items);
        free(seq);
    }
}


void releaseConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}

}

void releaseExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				releaseExpression(expression->leftExpression);
				releaseExpression(expression->rightExpression);
				break;
			case FACTOR:
				releaseFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void releaseFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				releaseConstant(factor->constant);
				break;
			case EXPRESSION:
				releaseExpression(factor->expression);
				break;
		}
		free(factor);
	}
}



void releaseProgram(Program *program) {
    if (program) {
        releaseSequence(program->body);
        free(program);
    }
}