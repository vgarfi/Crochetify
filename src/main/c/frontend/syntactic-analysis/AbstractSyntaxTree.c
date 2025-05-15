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
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (stitch) {
        free(stitch);
    }
}

void releaseParameter(Parameter *parameter) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parameter) {
        if (parameter->name) free(parameter->name);
        free(parameter);
    }
}

void releaseArgument(Argument *argument) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (argument) {
        if (argument->name) free(argument->name);
        free(argument);
    }
}

void releaseDeclaration(Declaration *declaration) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declaration) {
        if (declaration->typeName) free(declaration->typeName);
        if (declaration->varName) free(declaration->varName);
        if (declaration->value) free(declaration->value);
        free(declaration);
    }
}

void releaseAssignment(Assignment *assignment) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (assignment) {
        if (assignment->varName) free(assignment->varName);
        if (assignment->value) free(assignment->value);
        free(assignment);
    }
}

void releaseTurn(Turn *turn) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (turn) {
        if (turn->chains) releaseSequence(turn->chains);
        if (turn->color) free(turn->color);
        free(turn);
    }
}

void releasePatternUse(PatternUse *patternUse) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (patternUse) {
        if (patternUse->name) free(patternUse->name);
        if (patternUse->arguments) releaseSequence(patternUse->arguments);
        free(patternUse);
    }
}

void releaseRow(Row *row) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (row) {
        if (row->elements) releaseSequence(row->elements);
        if (row->color) free(row->color);
        free(row);
    }
}

void releaseRepeat(Repeat *repeat) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (repeat) {
        if (repeat->pattern) releaseSequence(repeat->pattern);
        free(repeat);
    }
}

void releaseMirror(Mirror *mirror) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (mirror) {
        if (mirror->pattern) releaseSequence(mirror->pattern);
        free(mirror);
    }
}

void releasePattern(Pattern *pattern) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (pattern) {
        if (pattern->name) free(pattern->name);
        if (pattern->parameters) releaseSequence(pattern->parameters);
        if (pattern->body) releaseSequence(pattern->body);
        free(pattern);
    }
}

void releaseColorRow(ColorRow *colorRow) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (colorRow) {
        if (colorRow->color) free(colorRow->color);
        if (colorRow->row) releaseRow(colorRow->row);
        free(colorRow);
    }
}

void releaseSequence(Sequence *seq) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
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
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program) {
        if (program->body) {
            releaseSequence(program->body);
        }
        free(program);
    }
}