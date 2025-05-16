#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

Parameter *ParameterSemanticAction(ParameterType type, char *name) {
    printf("[BisonActions] Creating Parameter: type=%d, name=%s\n", type, name);
    Parameter *p = calloc(1, sizeof(Parameter));
    p->type = ITEM_PARAMETER;
    p->paramType = type;
    p->name = strdup(name);
    return p;
}

Argument *ArgumentSemanticAction(char *name) {
    printf("[BisonActions] Creating Argument: name=%s\n", name);
    Argument *a = calloc(1, sizeof(Argument));
    a->type = ITEM_ARGUMENT;
    a->name = strdup(name);
    return a;
}

Declaration *DeclarationSemanticAction(char *typeName, char *varName, char *value) {
    printf("[BisonActions] Creating Declaration: type=%s, var=%s, value=%s\n", typeName, varName, value);
    Declaration *d = calloc(1, sizeof(Declaration));
    d->type = ITEM_DECLARATION;
    d->typeName = strdup(typeName);
    d->varName = strdup(varName);
    d->value = strdup(value);
    return d;
}

Assignment *AssignmentSemanticAction(char *varName, char *value) {
    printf("[BisonActions] Creating Assignment: var=%s, value=%s\n", varName, value);
    Assignment *a = calloc(1, sizeof(Assignment));
    a->type = ITEM_ASSIGNMENT;
    a->varName = strdup(varName);
    a->value = strdup(value);
    return a;
}

Stitch *StitchSemanticAction(StitchType type) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Stitch: type=%d\n", type);
    Stitch *stitch = calloc(1, sizeof(Stitch));
    stitch->type = ITEM_STITCH;
    stitch->stichType = type;
    return stitch;
}

Row *RowSemanticAction(Sequence *elements, char * color, int isTurn) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Row: color=%s, isTurn=%d\n", color ? color : "(null)", isTurn);
    Row *row = calloc(1, sizeof(Row));
    row->type = ITEM_ROW;
    row->elements = elements;
    if(color != NULL){
        row->color = strdup(color);
    } else {
        row->color = strdup("#000000");
    }
    row->isTurn = isTurn;
    return row;
}

Turn *TurnSemanticAction(Sequence * chains, char *color) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Turn: color=%s\n", color ? color : "(null)");
    Turn *turn = calloc(1, sizeof(Turn));
    turn->type = ITEM_TURN;
    turn->chains = chains;
    turn->color = color ? strdup(color) : NULL;
    return turn;
}

Repeat *RepeatSemanticAction(Sequence *pattern, int times) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Repeat: times=%d\n", times);
    Repeat *repeat = calloc(1, sizeof(Repeat));
    repeat->type = ITEM_REPEAT;
    repeat->pattern = pattern;
    repeat->times = times;
    return repeat;
}

Mirror *MirrorSemanticAction(Sequence *pattern, int times) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Mirror: times=%d\n", times);
    Mirror *mirror = calloc(1, sizeof(Mirror));
    mirror->type = ITEM_MIRROR;
    mirror->pattern = pattern;
    mirror->times = times;
    return mirror;
}

Pattern *PatternSemanticAction(char *name, Sequence *parameters, Sequence *body) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Pattern: name=%s\n", name);
    Pattern *pattern = calloc(1, sizeof(Pattern));
    pattern->type = ITEM_PATTERN;
    pattern->name = strdup(name);
    pattern->parameters = parameters;
    pattern->body = body;
    return pattern;
}

PatternUse *PatternUseSemanticAction(char *name, Sequence *arguments) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating PatternUse: name=%s\n", name);
    PatternUse *use = calloc(1, sizeof(PatternUse));
    use->type = ITEM_PATTERN_USE;
    use->name = strdup(name);
    use->arguments = arguments;
    return use;
}

ColorRow *ColorRowSemanticAction(char *color, Row *row) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating ColorRow: color=%s\n", color);
    ColorRow *cr = calloc(1, sizeof(ColorRow));
    cr->type = ITEM_COLOR_ROW;
    cr->color = strdup(color);
    cr->row = row;
    return cr;
}

Sequence *SequenceSemanticAction(void *item, ItemType itemType) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Sequence: itemType=%d\n", itemType);
    Sequence *seq = calloc(1, sizeof(Sequence));
    seq->items = calloc(1, sizeof(void*));
    seq->items[0] = item;
    seq->count = 1;
    seq->itemTypes = calloc(1, sizeof(ItemType));
    seq->itemTypes[0] = itemType;
    return seq;
}

Sequence *AppendToSequenceSemanticAction(Sequence *seq, void *item, ItemType itemType) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Appending to Sequence: itemType=%d, newCount=%d\n", itemType, seq ? seq->count + 1 : 1);
    if (seq == NULL) {
        return SequenceSemanticAction(item, itemType);
    }
    seq->items = realloc(seq->items, sizeof(void*) * (seq->count + 1));
    seq->itemTypes = realloc(seq->itemTypes, sizeof(ItemType) * (seq->count + 1));
    seq->items[seq->count] = item;
    seq->itemTypes[seq->count] = itemType;
    seq->count++;
    return seq;
}

Program *ProgramSemanticAction(CompilerState * compilerState, Sequence *declarationsAndPatterns, Sequence *body) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Program\n");
    Program *program = calloc(1, sizeof(Program));
    program->declarationsAndPatterns = declarationsAndPatterns;
    program->body = body;
    compilerState->abstractSyntaxtTree = program;
    if (0 < flexCurrentContext()) {
        printf("[BisonActions][ERROR] The final context is not the default (0): %d\n", flexCurrentContext());
        compilerState->succeed = false;
    }
    else {
        compilerState->succeed = true;
    }
    return program;
}

Constant * IntegerConstantSemanticAction(const int value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating Constant: value=%d\n", value);
    Constant * constant = calloc(1, sizeof(Constant));
    constant->value = value;
    return constant;
}

ItemType getItemType(void *item) {
    return *((ItemType*)item); // Seguro porque type es el primer campo
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating ArithmeticExpression: type=%d\n", type);
    Expression * expression = calloc(1, sizeof(Expression));
    expression->leftExpression = leftExpression;
    expression->rightExpression = rightExpression;
    expression->type = type;
    return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating ConstantFactor\n");
    Factor * factor = calloc(1, sizeof(Factor));
    factor->constant = constant;
    factor->type = CONSTANT;
    return factor;
}
Expression * FactorExpressionSemanticAction(Factor * factor) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating FactorExpression\n");
    Expression * expression = calloc(1, sizeof(Expression));
    expression->factor = factor;
    expression->type = FACTOR;
    return expression;
}
Factor * ExpressionFactorSemanticAction(Expression * expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    printf("[BisonActions] Creating ExpressionFactor\n");
    Factor * factor = calloc(1, sizeof(Factor));
    factor->expression = expression;
    factor->type = EXPRESSION;
    return factor;
}
/*
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->body = expression;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}
*/