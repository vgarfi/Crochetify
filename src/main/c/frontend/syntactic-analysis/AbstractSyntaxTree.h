#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

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
    ParameterType type;
    char *name;
};

struct Argument {
    char *name; // Puede ser nombre de variable o literal
};

struct Stitch {
    StitchType type;
};

struct Row {
    Sequence *elements; // lista de Stitch, Repeat, Mirror, PatternUse, ColorRow, etc.
};

struct Turn {
    int chains;
    char *color; // NULL si no hay color
};

struct Repeat {
    Sequence *pattern; // lo que se repite
    int times;
    Stitch *extra; // opcional, puede ser NULL, esto no va!
};

struct Mirror {
    Sequence *pattern;
    int times;
};

struct Pattern {
    char *name;
    Sequence *parameters; // lista de Parameter*
    Sequence *body;
};

struct PatternUse {
    char *name;
    Sequence *arguments; // lista de Argument*

};

struct ColorRow {
    char *color;
    Row *row;
};

struct Declaration {
    char *typeName; // "Color", "Stitch", etc.
    char *varName;
    char *value; // literal o nombre
};

struct Assignment {
    char *varName;
    char *value;
};

struct Sequence {
    void **items; // array de punteros a cualquier struct anterior
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
};

#endif

