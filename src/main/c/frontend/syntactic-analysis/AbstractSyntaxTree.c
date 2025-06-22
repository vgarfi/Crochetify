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
    [ITEM_PARAMETER]   = (ReleaseFunc)releaseParameter,
    [ITEM_ARGUMENT]    = (ReleaseFunc)releaseArgument,
    [ITEM_DECLARATION] = (ReleaseFunc)releaseDeclaration,
    [ITEM_ASSIGNMENT]  = (ReleaseFunc)releaseAssignment,
    [ITEM_IDENTIFIER] = free,
    [ITEM_COLOR_VALUE] = free,
    [ITEM_SEQUENCE] = (ReleaseFunc)releaseSequence
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
        free(stitch);
    }
}

void releaseParameter(Parameter *parameter) {
    if (parameter) {
        if (parameter->name) free(parameter->name);
        free(parameter);
    }
}

void releaseArgument(Argument *argument) {
    if (argument) {
        if(argument->value){
            releaseFuncs[argument->argumentType](argument->value);
        }
        free(argument);
    }
}

void releaseDeclaration(Declaration *declaration) {
    if (declaration) {
        if (declaration->typeName) free(declaration->typeName);
        if (declaration->varName) free(declaration->varName);
        if (declaration->value) free(declaration->value);
        free(declaration);
    }
}

void releaseAssignment(Assignment *assignment) {
    if (assignment) {
        if (assignment->varName) free(assignment->varName);
        if (assignment->value) free(assignment->value);
        free(assignment);
    }
}

void releasePatternUse(PatternUse *patternUse) {
    if (patternUse) {
        if (patternUse->name) free(patternUse->name);
        if (patternUse->arguments) releaseSequence(patternUse->arguments);
        free(patternUse);
    }
}

void releaseRow(Row *row) {
    if (row) {
        if (row->elements) releaseSequence(row->elements);
        if (row->color) free(row->color);
        free(row);
    }
}

void releaseTurn(Turn *turn) {
    if (turn) {
        if (turn->chains) releaseSequence(turn->chains);
        if (turn->color) free(turn->color);
        free(turn);
    }
}

void releaseRepeat(Repeat *repeat) {
    if (repeat) {
        if (repeat->pattern) releasePatternUse(repeat->pattern);
        free(repeat);
    }
}

void releaseMirror(Mirror *mirror) {
    if (mirror) {
        if (mirror->pattern) releasePatternUse(mirror->pattern);
        free(mirror);
    }
}

void releasePattern(Pattern *pattern) {
    if (pattern) {
        if (pattern->name) free(pattern->name);
        if (pattern->parameters) releaseSequence(pattern->parameters);
        if (pattern->body) releaseSequence(pattern->body);
        free(pattern);
    }
}

void releaseSequence(Sequence *seq) {
    if (seq) {
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

void releaseProgram(Program *program) {
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