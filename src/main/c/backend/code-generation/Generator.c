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
/**
 * Converts and expression type to the proper character of the operation
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

/**
 * Generates the output of a constant.
 */
static void _generateConstant(const unsigned int indentationLevel, Constant * constant) {
	_output(indentationLevel, "%s", "[ $C$, circle, draw, black!20\n");
	_output(1 + indentationLevel, "%s%d%s", "[ $", constant->value, "$, circle, draw ]\n");
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Creates the epilogue of the generated output, that is, the final lines that
 * completes a valid Latex document.
 */
static void _generateEpilogue(const int value) {
	_output(0, "%s%d%s",
		"            [ $", value, "$, circle, draw, blue ]\n"
		"        ]\n"
		"    \\end{forest}\n"
		"\\end{document}\n\n"
	);
}

/**
 * Generates the output of an expression.
 */
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

/**
 * Generates the output of a factor.
 */
static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	_output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
	switch (factor->type) {
		case CONSTANT:
			_generateConstant(1 + indentationLevel, factor->constant);
			break;
		case EXPRESSION:
			_output(1 + indentationLevel, "%s", "[ $($, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, factor->expression);
			_output(1 + indentationLevel, "%s", "[ $)$, circle, draw, purple ]\n");
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}


static void _generateStitch(Stitch *stitch) {
    if (!stitch) return;
    const char *name = stitch->stichType == STITCH_CH ? "CH" :
                       stitch->stichType == STITCH_SC ? "SC" : "DC";
    printf("[Stitch(%s)]", name);
}
static void _generateArgument(Argument *arg) {
    if (!arg) return;
    printf("Argument(%s)", arg->name);
}



static void _generateRow(Row *row) {
    if (!row) return;
    printf("[Row color=%s, isTurn=%d", row->color ? row->color : "#000000", row->isTurn);
    if (row->elements) _generateSequence(row->elements);
    printf("]");
}

static void _generatePatternUse(PatternUse *use) {
    if (!use) return;
    printf("PatternUse(name=%s, arguments=[", use->name);
    if (use->arguments) _generateSequence(use->arguments);
    printf("])");
}

static void _generatePattern(Pattern *pattern) {
    if (!pattern) return;
    printf("Pattern(name=%s, parameters=[", pattern->name);
    if (pattern->parameters) _generateSequence(pattern->parameters);
    printf("], body=[");
    if (pattern->body) _generateSequence(pattern->body);
    printf("])");
}

static void _generateDeclaration(Declaration *decl) {
    if (!decl) return;
    printf("Declaration(type=%s, var=%s, value=%s)", decl->typeName, decl->varName, decl->value);
}

static void _generateAssignment(Assignment *assign) {
    if (!assign) return;
    printf("Assignment(var=%s, value=%s)", assign->varName, assign->value);
}

static void _generateSequence(Sequence *seq) {
    if (!seq) return;
    for (int i = 0; i < seq->count; ++i) {
        printf("\n");
        switch (seq->itemTypes[i]) {
            case ITEM_STITCH:
                printf("  ");
                _generateStitch((Stitch*)seq->items[i]);
                break;
            case ITEM_ROW:
                printf("  ");
                _generateRow((Row*)seq->items[i]);
                break;
            case ITEM_TURN:
                printf("  ");
                _generateTurn((Turn*)seq->items[i]);
                break;
            case ITEM_REPEAT:
                printf("  ");
                _generateRepeat((Repeat*)seq->items[i]);
                break;
            case ITEM_MIRROR:
                printf("  [Mirror]");
                break;
            case ITEM_PATTERN:
                printf("  ");
                _generatePattern((Pattern*)seq->items[i]);
                break;
            case ITEM_PATTERN_USE:
                printf("  ");
                _generatePatternUse((PatternUse*)seq->items[i]);
                break;
            case ITEM_COLOR_ROW:
                printf("  [ColorRow]");
                break;
            case ITEM_PARAMETER:
                printf("  [Parameter]");
                break;
            case ITEM_ARGUMENT:
                printf("  ");
                _generateArgument((Argument*)seq->items[i]);
                break;
            case ITEM_DECLARATION:
                printf("  ");
                _generateDeclaration((Declaration*)seq->items[i]);
                break;
            case ITEM_ASSIGNMENT:
                printf("  ");
                _generateAssignment((Assignment*)seq->items[i]);
                break;
            default:
                printf("  [UnknownItem]");
                break;
        }
    }
}
static void _generateTurn(Turn *turn) {
    if (!turn) return;
    printf("[Turn");
    if (turn->chains) {
        printf(" chains=");
        _generateSequence(turn->chains);
    }
    if (turn->color) {
        printf(" color=%s", turn->color);
    }
    printf("]");
}

static void _generateRepeat(Repeat *repeat) {
    if (!repeat) return;
    printf("[Repeat");
    if (repeat->pattern) {
        printf(" pattern=");
        _generateSequence(repeat->pattern);
    }
	
    printf(" count=%d", repeat->times); // Assuming you have a count field
    printf("]");
}

/**
 * Generates the output of the program.
 */
static void _generateProgram(Program *program) {
    printf("[Program\n");
    printf("[declarationsAndPatterns\n");
    if (program->declarationsAndPatterns) _generateSequence(program->declarationsAndPatterns);
    printf("]\n[body\n");
    if (program->body) _generateSequence(program->body);
    printf("]\n]\n");
}

/**
 * Creates the prologue of the generated output, a Latex document that renders
 * a tree thanks to the Forest package.
 *
 * @see https://ctan.dcc.uchile.cl/graphics/pgf/contrib/forest/forest-doc.pdf
 */
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

/**
 * Generates an indentation string for the specified level.
 */
static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
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

/** PUBLIC FUNCTIONS */

void generate(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
    Program *program = (Program*)compilerState->abstractSyntaxtTree;
    _generateProgram(program);
	_generateEpilogue(compilerState->value);
	logDebugging(_logger, "Generation is done.");
}

