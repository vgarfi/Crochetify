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


void releaseStitch(Stitch *stitch) {
    printf("[AST] Executing destructor: releaseStitch\n");
    if (stitch) {
        free(stitch);
    }
}

void releaseParameter(Parameter *parameter) {
    printf("[AST] Executing destructor: releaseParameter (name=%s)\n", parameter && parameter->name ? parameter->name : "(null)");
    if (parameter) {
        if (parameter->name) free(parameter->name);
        free(parameter);
    }
}

void releaseArgument(Argument *argument) {
    printf("[AST] Executing destructor: releaseArgument (name=%s)\n", argument && argument->name ? argument->name : "(null)");
    if (argument) {
        if (argument->name) free(argument->name);
        free(argument);
    }
}

void releaseDeclaration(Declaration *declaration) {
    printf("[AST] Executing destructor: releaseDeclaration (var=%s)\n", declaration && declaration->varName ? declaration->varName : "(null)");
    if (declaration) {
        if (declaration->typeName) free(declaration->typeName);
        if (declaration->varName) free(declaration->varName);
        if (declaration->value) free(declaration->value);
        free(declaration);
    }
}

void releaseAssignment(Assignment *assignment) {
    printf("[AST] Executing destructor: releaseAssignment (var=%s)\n", assignment && assignment->varName ? assignment->varName : "(null)");
    if (assignment) {
        if (assignment->varName) free(assignment->varName);
        if (assignment->value) free(assignment->value);
        free(assignment);
    }
}

void releaseTurn(Turn *turn) {
    printf("[AST] Executing destructor: releaseTurn\n");
    if (turn) {
        if (turn->chains) releaseSequence(turn->chains);
        if (turn->color) free(turn->color);
        free(turn);
    }
}

void releasePatternUse(PatternUse *patternUse) {
    printf("[AST] Executing destructor: releasePatternUse (name=%s)\n", patternUse && patternUse->name ? patternUse->name : "(null)");
    if (patternUse) {
        if (patternUse->name) free(patternUse->name);
        if (patternUse->arguments) releaseSequence(patternUse->arguments);
        free(patternUse);
    }
}

void releaseRow(Row *row) {
    printf("[AST] Executing destructor: releaseRow\n");
    if (row) {
        if (row->elements) releaseSequence(row->elements);
        if (row->color) free(row->color);
        free(row);
    }
}

void releaseRepeat(Repeat *repeat) {
    printf("[AST] Executing destructor: releaseRepeat\n");
    if (repeat) {
        if (repeat->pattern) releaseSequence(repeat->pattern);
        free(repeat);
    }
}

void releaseMirror(Mirror *mirror) {
    printf("[AST] Executing destructor: releaseMirror\n");
    if (mirror) {
        if (mirror->pattern) releaseSequence(mirror->pattern);
        free(mirror);
    }
}

void releasePattern(Pattern *pattern) {
    printf("[AST] Executing destructor: releasePattern (name=%s)\n", pattern && pattern->name ? pattern->name : "(null)");
    if (pattern) {
        if (pattern->name) free(pattern->name);
        if (pattern->parameters) releaseSequence(pattern->parameters);
        if (pattern->body) releaseSequence(pattern->body);
        free(pattern);
    }
}

void releaseColorRow(ColorRow *colorRow) {
    printf("[AST] Executing destructor: releaseColorRow\n");
    if (colorRow) {
        if (colorRow->color) free(colorRow->color);
        if (colorRow->row) releaseRow(colorRow->row);
        free(colorRow);
    }
}

void releaseSequence(Sequence *seq) {
    printf("[AST] Executing destructor: releaseSequence (count=%d)\n", seq ? seq->count : 0);
    if (seq) {
        if (seq->items && seq->itemTypes) {
            for (int i = 0; i < seq->count; ++i) {
                if (seq->items[i] == NULL) continue;
                switch(seq->itemTypes[i]) {
                    case ITEM_STITCH:
                        releaseStitch((Stitch*)seq->items[i]);
                        break;
                    case ITEM_ROW:
                        releaseRow((Row*)seq->items[i]);
                        break;
                    case ITEM_TURN:
                        releaseTurn((Turn*)seq->items[i]);
                        break;
                    case ITEM_REPEAT:
                        releaseRepeat((Repeat*)seq->items[i]);
                        break;
                    case ITEM_MIRROR:
                        releaseMirror((Mirror*)seq->items[i]);
                        break;
                    case ITEM_PATTERN:
                        releasePattern((Pattern*)seq->items[i]);
                        break;
                    case ITEM_PATTERN_USE:
                        releasePatternUse((PatternUse*)seq->items[i]);
                        break;
                    case ITEM_COLOR_ROW:
                        releaseColorRow((ColorRow*)seq->items[i]);
                        break;
                    case ITEM_PARAMETER:
                        releaseParameter((Parameter*)seq->items[i]);
                        break;
                    case ITEM_ARGUMENT:
                        releaseArgument((Argument*)seq->items[i]);
                        break;
                    case ITEM_DECLARATION:
                        releaseDeclaration((Declaration*)seq->items[i]);
                        break;
                    case ITEM_ASSIGNMENT:
                        releaseAssignment((Assignment*)seq->items[i]);
                        break;
                }
            }
        }
        if (seq->items) free(seq->items);
        if (seq->itemTypes) free(seq->itemTypes);
        free(seq);
    }
}

void releaseConstant(Constant * constant) {
    printf("[AST] Executing destructor: releaseConstant\n");
    if (constant != NULL) {
        free(constant);
    }
}

void releaseExpression(Expression * expression) {
    printf("[AST] Executing destructor: releaseExpression\n");
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
    printf("[AST] Executing destructor: releaseFactor\n");
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
    printf("[AST] Executing destructor: releaseProgram\n");
    if (program) {
        if (program->declarationsAndPatterns) {
            releaseSequence(program->declarationsAndPatterns);
        }
        if (program->body) {
            releaseSequence(program->body);
        }
        free(program);
    }
}