#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#define ISTURN 1
#define ISNOTTURN 0
#define COLOR_NAME "Color"
#define STITCH_NAME "Stitch"

#include "../../shared/Logger.h"
#include <stdlib.h>

typedef struct Stitch Stitch;
typedef struct Row Row;
typedef struct Turn Turn;
typedef struct Repeat Repeat;
typedef struct Mirror Mirror;
typedef struct Pattern Pattern;
typedef struct PatternUse PatternUse;
typedef struct ColorRow ColorRow;
typedef struct Sequence Sequence;
typedef struct Parameter Parameter;
typedef struct Argument Argument;
typedef struct Declaration Declaration;
typedef struct Assignment Assignment;
typedef struct Program Program;

void initializeAbstractSyntaxTreeModule();
void shutdownAbstractSyntaxTreeModule();

void releaseProgram(Program *program);
void releaseRepeat(Repeat *repeat);
void releaseMirror(Mirror *mirror);
void releasePattern(Pattern *pattern);
void releaseSequence(Sequence *seq);
void releaseRow(Row *row);
void releaseTurn(Turn *turn);
void releaseStitch(Stitch *stitch);
void releaseParameter(Parameter *parameter);
void releaseArgument(Argument *argument);
void releaseDeclaration(Declaration *declaration);
void releaseAssignment(Assignment *assignment);
void releasePatternUse(PatternUse *patternUse);

typedef enum {
    ITEM_STITCH,
    ITEM_ROW,
    ITEM_TURN,
    ITEM_REPEAT,
    ITEM_MIRROR,
    ITEM_PATTERN,
    ITEM_PATTERN_USE,
    ITEM_PARAMETER,     // 7
    ITEM_ARGUMENT,      // 8
    ITEM_DECLARATION,
    ITEM_ASSIGNMENT,
    ITEM_IDENTIFIER,
    ITEM_COLOR_VALUE,
    ITEM_SEQUENCE
} ItemType;

struct Parameter {
    ItemType type;
    VariableType paramType;
    char *name;
};

struct Argument {
    ItemType type;
    ItemType argumentType;
    void * value;
};

struct Stitch {
    ItemType type;
    StitchType stitchType;
};

struct Row {
    ItemType type;
    Sequence *elements;
    char * color;
    int isTurn;
};

struct Turn {
    ItemType type;
    Sequence * chains;
    char *color;
};

struct Repeat {
    ItemType type;
    PatternUse *pattern;
    int times;
};

struct Mirror {
    ItemType type;
    PatternUse *pattern;
    int times;
};

struct Pattern {
    ItemType type;
    char *name;
    Sequence *parameters;
    Sequence *body;
};

struct PatternUse {
    ItemType type;
    char *name;
    Sequence *arguments;
};

struct ColorRow {
     ItemType type;
    char *color;
    Row *row;
};

struct Declaration {
    ItemType type;
    char *typeName;
    char *varName;
    char *value;
};

struct Assignment {
    ItemType type;
    char *varName;
    char *value;
};

struct Sequence {
    void **items;
    ItemType * itemTypes;
    int count;
};

struct Program {
    Sequence * body;
    Sequence *declarationsAndPatterns; 
};

#endif
