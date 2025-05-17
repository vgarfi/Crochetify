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

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

void initializeAbstractSyntaxTreeModule();
void shutdownAbstractSyntaxTreeModule();

void releaseProgram(Program *program);
void releaseRepeat(Repeat *repeat);
void releaseMirror(Mirror *mirror);
void releasePattern(Pattern *pattern);
void releaseSequence(Sequence *seq);
void releaseRow(Row *row);
void releaseStitch(Stitch *stitch);
void releaseParameter(Parameter *parameter);
void releaseArgument(Argument *argument);
void releaseDeclaration(Declaration *declaration);
void releaseAssignment(Assignment *assignment);
void releasePatternUse(PatternUse *patternUse);

// TODO: REMOVE
void releaseColorRow(ColorRow *colorRow);
void releaseTurn(Turn *turn);
void releaseConstant(Constant *constant);
void releaseExpression(Expression *expression);
void releaseFactor(Factor *factor);

typedef enum {
    ITEM_STITCH,
    ITEM_ROW,
    ITEM_TURN,
    ITEM_REPEAT,
    ITEM_MIRROR,
    ITEM_PATTERN,
    ITEM_PATTERN_USE,
    ITEM_PARAMETER,
    ITEM_ARGUMENT,
    ITEM_DECLARATION,
    ITEM_ASSIGNMENT,
    ITEM_IDENTIFIER,
    ITEM_COLOR_VALUE,
    ITEM_SEQUENCE
} ItemType;

typedef enum {
    STITCH_CH,
    STITCH_SC,
    STITCH_DC
} StitchType;

typedef enum {
    PARAM_COLOR,
    PARAM_STITCH,
    PARAM_PATTERN
} ParameterType;

struct Parameter {
    ItemType type;
    ParameterType paramType;
    char *name;
};

struct Argument {
    ItemType type;
    ItemType argumentType;
    void * value;
};

struct Stitch {
    ItemType type;
    StitchType stichType;
};

struct Row {
    ItemType type;
    Sequence *elements; // lista de Stitch, Repeat, Mirror, PatternUse, ColorRow, etc.
    char * color;
    int isTurn;
};

struct Turn {
    ItemType type;
    Sequence * chains;
    char *color; // NULL si no hay color
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
    Sequence *parameters; // lista de Parameter*
    Sequence *body;
};

struct PatternUse {
     ItemType type;
    char *name;
    Sequence *arguments; // lista de Argument*

};

struct ColorRow {
     ItemType type;
    char *color;
    Row *row;
};

struct Declaration {
    ItemType type;
    char *typeName; // "Color", "Stitch", etc.
    char *varName;
    char *value; // literal o nombre
};

struct Assignment {
    ItemType type;
    char *varName;
    char *value;
};

struct Sequence {
    void **items; // array de punteros a cualquier struct anterior
    ItemType * itemTypes;
    int count;
};


enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};
enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};
struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
    union {
        Sequence *body; // lista de Row, PatternUse, Pattern, etc.
        Expression * expression; // TODO esto vuela con la calculadora
    };
        Sequence *declarationsAndPatterns; // lo que va antes del main

};

#endif
