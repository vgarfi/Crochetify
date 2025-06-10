#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */
static void printPatternUse(PatternUse *use, int indent);
static const char _expressionTypeToCharacter(const ExpressionType type);
static void _generateConstant(const unsigned int indentationLevel, Constant * constant);
static void _generateEpilogue(const int value);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);
static void _generateProgram(Program * program);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static void _generateSequence(Sequence *seq);
static void _generateRepeat(Repeat *repeat);
static void _generateTurn(Turn *turn); 
static void printSequence(Sequence *seq, int indent);

static const char * itemTypeToString(ItemType type) {
    switch (type) {
        case ITEM_STITCH: return "Stitch";
        case ITEM_ROW: return "Row";
        case ITEM_TURN: return "Turn";
        case ITEM_REPEAT: return "Repeat";
        case ITEM_MIRROR: return "Mirror";
        case ITEM_PATTERN: return "Pattern";
        case ITEM_PATTERN_USE: return "PatternUse";
        case ITEM_PARAMETER: return "Parameter";
        case ITEM_ARGUMENT: return "Argument";
        case ITEM_DECLARATION: return "Declaration";
        case ITEM_ASSIGNMENT: return "Assignment";
        case ITEM_IDENTIFIER: return "Identifier";
        case ITEM_COLOR_VALUE: return "ColorValue";
        case ITEM_SEQUENCE: return "Sequence";
        default: return "Unknown";
    }
}
static const char * parameterTypeToString(ParameterType type) {
    switch (type) {
        case PARAM_COLOR: return "Color";
        case PARAM_STITCH: return "Stitch";
        case PARAM_PATTERN: return "Pattern";
        default: return "Unknown";
    }
}
/**
 * Converts an expression type to the proper character of the operation
 * involved, or returns '\0' if that's not possible.
 */
static const char _expressionTypeToCharacter(const ExpressionType type) {
	switch (type) {
		case ADDITION: return '+';
		case DIVISION: return '/';
		case MULTIPLICATION: return '*';
		case SUBTRACTION: return '-';
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return '\0';
	}
}

static void _generateConstant(const unsigned int indentationLevel, Constant * constant) {
	_output(indentationLevel, "%s", "[ $C$, circle, draw, black!20\n");
	_output(1 + indentationLevel, "%s%d%s", "[ $", constant->value, "$, circle, draw ]\n");
	_output(indentationLevel, "%s", "]\n");
}

static void _generateEpilogue(const int value) {
	_output(0, "%s%d%s",
		"            [ $", value, "$, circle, draw, blue ]\n"
		"        ]\n"
		"    \\end{forest}\n"
		"\\end{document}\n\n"
	);
}

static void _generateExpression(const unsigned int indentationLevel, Expression * expression) {
	_output(indentationLevel, "%s", "[ $E$, circle, draw, black!20\n");
	switch (expression->type) {
		case ADDITION:
		case DIVISION:
		case MULTIPLICATION:
		case SUBTRACTION:
			_generateExpression(1 + indentationLevel, expression->leftExpression);
			_output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			break;
		case FACTOR:
			_generateFactor(1 + indentationLevel, expression->factor);
			break;
		default:
			logError(_logger, "The specified expression type is unknown: %d", expression->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}


static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	_output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
	switch (factor->type) {
		case CONSTANT:
			_generateConstant(1 + indentationLevel, factor->constant);
			break;
		case EXPRESSION:
			_output(1 + indentationLevel, "%s", "[ $(, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, factor->expression);
			_output(1 + indentationLevel, "%s", "[ $), circle, draw, purple ]\n");
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

static void printStitch(Stitch *stitch, int indent) {
    if (!stitch) return;
    const char *name = stitch->stichType == STITCH_CH ? "CH" :
                       stitch->stichType == STITCH_SC ? "SC" : "DC";
    printIndent(indent); printf("Stitch(%s)\n", name);
}

static void printParameter(Parameter *param, int indent) {
    if (!param) return;
    printIndent(indent);
    printf("Parameter(name=%s, type=%s)\n",
           param->name,
           parameterTypeToString(param->paramType));
}

static void printArgument(Argument *arg, int indent) {
    if (!arg) return;

    printIndent(indent);
    printf("Argument(type=%s, valueType=%s)\n",
           itemTypeToString(arg->type),
           itemTypeToString(arg->argumentType));

    if (arg->value) {
        printIndent(indent + 2);
        printf("value: ");
        switch (arg->argumentType) {
            case ITEM_IDENTIFIER:
                printf("%s\n", (char*)arg->value);
                break;
            case ITEM_COLOR_VALUE:
                printf("%s\n", (char*)arg->value);
                break;
            case ITEM_STITCH:
                printStitch((Stitch *)arg->value, 0);
                break;
            case ITEM_PATTERN_USE:
                printPatternUse((PatternUse *)arg->value, indent + 2);
                break;
            default:
                printf("0x%p\n", arg->value);
                break;
        }
    }
}

static void printRow(Row *row, int indent) {
    if (!row) return;
    printIndent(indent);
    printf("Row color=%s, isTurn=%d\n", row->color ? row->color : "#000000", row->isTurn);
    if (row->elements) printSequence(row->elements, indent + 2);
}

static void printPatternUse(PatternUse *use, int indent) {
    if (!use) return;
    printIndent(indent); printf("PatternUse name=%s\n", use->name);
    if (use->arguments) printSequence(use->arguments, indent + 2);
}

static void printPattern(Pattern *pattern, int indent) {
    if (!pattern) return;
    printIndent(indent); printf("Pattern name=%s\n", pattern->name);
    if (pattern->parameters) {
        printIndent(indent + 2); printf("parameters:\n");
        printSequence(pattern->parameters, indent + 4);
    }
    if (pattern->body) {
        printIndent(indent + 2); printf("body:\n");
        printSequence(pattern->body, indent + 4);
    }
}

static void printDeclaration(Declaration *decl, int indent) {
    if (!decl) return;
    printIndent(indent);
    printf("Declaration(type=%s, var=%s, value=%s)\n", decl->typeName, decl->varName, decl->value);
}

static void printAssignment(Assignment *assign, int indent) {
    if (!assign) return;
    printIndent(indent);
    printf("Assignment(var=%s, value=%s)\n", assign->varName, assign->value);
}

static void printTurn(Turn *turn, int indent) {
    if (!turn) return;
    printIndent(indent); printf("Turn");
    if (turn->chains) {
        printf(" chains:\n");
        printSequence(turn->chains, indent + 2);
    }
    if (turn->color) {
        printf(" color=%s", turn->color);
    }
    printf("\n");
}

static void printRepeat(Repeat *repeat, int indent) {
    if (!repeat) return;
    printIndent(indent); printf("Repeat count=%d\n", repeat->times);
    if (repeat->pattern) printPatternUse(repeat->pattern, indent + 2);
}


static void printSequence(Sequence *seq, int indent) {
    if (!seq) return;
    for (int i = 0; i < seq->count; ++i) {
        switch (seq->itemTypes[i]) {
            case ITEM_STITCH:
                printStitch((Stitch*)seq->items[i], indent);
                break;
            case ITEM_ROW:
                printRow((Row*)seq->items[i], indent);
                break;
            case ITEM_TURN:
                printTurn((Turn*)seq->items[i], indent);
                break;
            case ITEM_REPEAT:
                printRepeat((Repeat*)seq->items[i], indent);
                break;
            case ITEM_MIRROR:
                printIndent(indent); printf("Mirror\n");
                break;
            case ITEM_PATTERN:
                printPattern((Pattern*)seq->items[i], indent);
                break;
            case ITEM_PATTERN_USE:
                printPatternUse((PatternUse*)seq->items[i], indent);
                break;
            case ITEM_PARAMETER:
                printParameter((Parameter*)seq->items[i], indent);
                break;
            case ITEM_ARGUMENT:
                printArgument((Argument*)seq->items[i], indent);
                break;
            case ITEM_DECLARATION:
                printDeclaration((Declaration*)seq->items[i], indent);
                break;
            case ITEM_ASSIGNMENT:
                printAssignment((Assignment*)seq->items[i], indent);
                break;
            default:
                printIndent(indent); printf("UnknownItem\n");
                break;
        }
    }
}

static void printProgram(Program *program, int indent) {
    printIndent(indent); printf("Program\n");
    printIndent(indent + 2); printf("declarationsAndPatterns:\n");
    if (program->declarationsAndPatterns) printSequence(program->declarationsAndPatterns, indent + 4);
    printIndent(indent + 2); printf("body:\n");
    if (program->body) printSequence(program->body, indent + 4);
}

static void _generatePrologue(void) {
	_output(0, "%s",
		"\\documentclass{standalone}\n\n"
		"\\usepackage[utf8]{inputenc}\n"
		"\\usepackage[T1]{fontenc}\n"
		"\\usepackage{amsmath}\n"
		"\\usepackage{forest}\n"
		"\\usepackage{microtype}\n\n"
		"\\begin{document}\n"
		"    \\centering\n"
		"    \\begin{forest}\n"
		"        [ \\text{$=$}, circle, draw, purple\n"
	);
}

static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

static void _output(const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	fflush(stdout);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

void generate(CompilerState * compilerState) {
    logDebugging(_logger, "Generating final output...");
    Program *program = (Program*)compilerState->abstractSyntaxtTree;
    printProgram(program, 0);
    logDebugging(_logger, "Generation is done.");
}
