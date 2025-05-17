#include "AbstractSyntaxTree.h"

static Logger * _logger = NULL;
typedef void (*ReleaseFunc)(void *);

static ReleaseFunc releaseFuncs[] = {
    [ITEM_STITCH]      = (ReleaseFunc)releaseStitch,
    [ITEM_ROW]         = (ReleaseFunc)releaseRow,
    [ITEM_TURN]        = (ReleaseFunc)releaseTurn,
    [ITEM_REPEAT]      = (ReleaseFunc)releaseRepeat,
    [ITEM_MIRROR]      = (ReleaseFunc)releaseMirror,
    [ITEM_PATTERN]     = (ReleaseFunc)releasePattern,
    [ITEM_PATTERN_USE] = (ReleaseFunc)releasePatternUse,
    [ITEM_COLOR_ROW]   = (ReleaseFunc)releaseColorRow,
    [ITEM_PARAMETER]   = (ReleaseFunc)releaseParameter,
    [ITEM_ARGUMENT]    = (ReleaseFunc)releaseArgument,
    [ITEM_DECLARATION] = (ReleaseFunc)releaseDeclaration,
    [ITEM_ASSIGNMENT]  = (ReleaseFunc)releaseAssignment
};

void initializeAbstractSyntaxTreeModule() {
    _logger = createLogger("AbstractSyntaxTree");
}

void shutdownAbstractSyntaxTreeModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

void releaseStitch(Stitch *stitch) {
    if (stitch) {
        printf("[AST] Releasing Stitch: type=%d\n", stitch->stichType);
        free(stitch);
    }
}

void releaseParameter(Parameter *parameter) {
    if (parameter) {
        printf("[AST] Releasing Parameter: type=%d, name=%s\n", parameter->paramType, parameter->name ? parameter->name : "(null)");
        if (parameter->name) free(parameter->name);
        free(parameter);
    }
}

void releaseArgument(Argument *argument) {
    if (argument) {
        printf("[AST] Releasing Argument: name=%s\n", argument->name ? argument->name : "(null)");
        if (argument->anon) releasePatternUse(argument->anon);
        if (argument->name) free(argument->name);
        free(argument);
    }
}

void releaseDeclaration(Declaration *declaration) {
    if (declaration) {
        printf("[AST] Releasing Declaration: type=%s, var=%s, value=%s\n",
            declaration->typeName ? declaration->typeName : "(null)",
            declaration->varName ? declaration->varName : "(null)",
            declaration->value ? declaration->value : "(null)");
        if (declaration->typeName) free(declaration->typeName);
        if (declaration->varName) free(declaration->varName);
        if (declaration->value) free(declaration->value);
        free(declaration);
    }
}

void releaseAssignment(Assignment *assignment) {
    if (assignment) {
        printf("[AST] Releasing Assignment: var=%s, value=%s\n",
            assignment->varName ? assignment->varName : "(null)",
            assignment->value ? assignment->value : "(null)");
        if (assignment->varName) free(assignment->varName);
        if (assignment->value) free(assignment->value);
        free(assignment);
    }
}

void releaseTurn(Turn *turn) {
    if (turn) {
        printf("[AST] Releasing Turn: color=%s\n", turn->color ? turn->color : "(null)");
        if (turn->chains) releaseSequence(turn->chains);
        if (turn->color) free(turn->color);
        free(turn);
    }
}

void releasePatternUse(PatternUse *patternUse) {
    if (patternUse) {
        printf("[AST] Releasing PatternUse: name=%s\n", patternUse->name ? patternUse->name : "(null)");
        if (patternUse->name) free(patternUse->name);
        if (patternUse->arguments) releaseSequence(patternUse->arguments);
        free(patternUse);
    }
}

void releaseRow(Row *row) {
    if (row) {
        printf("[AST] Releasing Row: color=%s, isTurn=%d\n", row->color ? row->color : "(null)", row->isTurn);
        if (row->elements) releaseSequence(row->elements);
        if (row->color) free(row->color);
        free(row);
    }
}

void releaseRepeat(Repeat *repeat) {
    if (repeat) {
        printf("[AST] Releasing Repeat: times=%d\n", repeat->times);
        if (repeat->pattern) releaseSequence(repeat->pattern);
        free(repeat);
    }
}

void releaseMirror(Mirror *mirror) {
    if (mirror) {
        printf("[AST] Releasing Mirror: times=%d\n", mirror->times);
        if (mirror->pattern) releaseSequence(mirror->pattern);
        free(mirror);
    }
}

void releasePattern(Pattern *pattern) {
    if (pattern) {
        printf("[AST] Releasing Pattern: name=%s\n", pattern->name ? pattern->name : "(null)");
        if (pattern->name) free(pattern->name);
        if (pattern->parameters) releaseSequence(pattern->parameters);
        if (pattern->body) releaseSequence(pattern->body);
        free(pattern);
    }
}

void releaseColorRow(ColorRow *colorRow) {
    if (colorRow) {
        printf("[AST] Releasing ColorRow: color=%s\n", colorRow->color ? colorRow->color : "(null)");
        if (colorRow->color) free(colorRow->color);
        if (colorRow->row) releaseRow(colorRow->row);
        free(colorRow);
    }
}

void releaseSequence(Sequence *seq) {
    if (seq) {
        printf("[AST] Releasing Sequence: count=%d\n", seq->count);
        if (seq->items && seq->itemTypes) {
            for (int i = 0; i < seq->count; ++i) {
                if (seq->items[i] == NULL) continue;
                ItemType type = seq->itemTypes[i];
                if (type >= 0 && type < (sizeof(releaseFuncs)/sizeof(releaseFuncs[0])) && releaseFuncs[type]) {
                    releaseFuncs[type](seq->items[i]);
                }
            }
        }
        if (seq->items) free(seq->items);
        if (seq->itemTypes) free(seq->itemTypes);
        free(seq);
    }
}
void releaseConstant(Constant * constant) {
    if (constant != NULL) {
        printf("[AST] Releasing Constant: value=%d\n", constant->value);
        free(constant);
    }
}

void releaseExpression(Expression * expression) {
    if (expression != NULL) {
        printf("[AST] Releasing Expression: type=%d\n", expression->type);
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
    if (factor != NULL) {
        printf("[AST] Releasing Factor: type=%d\n", factor->type);
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
        printf("[AST] Releasing Program\n");
        if (program->declarationsAndPatterns) {
            releaseSequence(program->declarationsAndPatterns);
        }
        if (program->body) {
            releaseSequence(program->body);
        }
        free(program);
    }
}