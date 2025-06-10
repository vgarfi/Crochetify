#include "BisonActions.h"
#include "../../shared/structures/ScopeListADT.h"

static RowNodeListADT _rowNodeList = NULL;
static ScopeListADT _scopeList = NULL;

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
    // _rowNodeList = newRowNodeList();
    // _scopeList = newScopeList();
    // insertNewScope(_scopeList, NULL);
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
    if(_rowNodeList != NULL){
        // freeRowNodeList(_rowNodeList);
    }
    if(_scopeList != NULL){
        // freeScopeList(_scopeList);
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

ItemType getItemType(void *item) {
    return *((ItemType*)item);
}

char * getStitchValue(StitchType type){
    char * stitch = malloc(3);
    stitch[2] = '\0';
    if(type == STITCH_CH){
        strcpy(stitch, "CH");
    } else if(type == STITCH_SC){
        strcpy(stitch, "SC");
    } else {
        strcpy(stitch, "DC");
    }
    return stitch;
}

Parameter *ParameterSemanticAction(ParameterType type, char *name) {
    printf("[BisonActions] Creating Parameter: type=%d (%s), name=%s\n", type,
        type == PARAM_COLOR ? "Color" : type == PARAM_STITCH ? "Stitch" : "Pattern",
        name ? name : "(null)");
    Parameter *p = calloc(1, sizeof(Parameter));
    p->type = ITEM_PARAMETER;
    p->paramType = type;
    p->name = strdup(name);
    return p;
}


Argument *ArgumentSemanticAction(void *argumentValue, ItemType argumentType) {
    printf("[BisonActions] Creating Argument: type=%d\n", argumentType);
    Argument *a = calloc(1, sizeof(Argument));
    a->type = ITEM_ARGUMENT;
    a->argumentType = argumentType;
    a->value = argumentValue;
    return a;
}

Declaration *DeclarationSemanticAction(char *typeName, char *varName, char *value) {
    printf("[BisonActions] Creating Declaration: type=%s, var=%s, value=%s\n",
        typeName ? typeName : "(null)",
        varName ? varName : "(null)",
        value ? value : "(null)");
    Declaration *d = calloc(1, sizeof(Declaration));
    d->type = ITEM_DECLARATION;
    d->typeName = strdup(typeName);
    d->varName = strdup(varName);
    d->value = strdup(value);
    return d;
}

Assignment *AssignmentSemanticAction(char *varName, char *value) {
    printf("[BisonActions] Creating Assignment: var=%s, value=%s\n",
        varName ? varName : "(null)",
        value ? value : "(null)");
    Assignment *a = calloc(1, sizeof(Assignment));
    a->type = ITEM_ASSIGNMENT;
    a->varName = strdup(varName);
    a->value = strdup(value);
    return a;
}

Stitch *StitchSemanticAction(StitchType type) {
    printf("[BisonActions] Creating Stitch: type=%d (%s)\n", type,
        type == STITCH_CH ? "CH" : type == STITCH_SC ? "SC" : "DC");
    Stitch *stitch = calloc(1, sizeof(Stitch));
    stitch->type = ITEM_STITCH;
    stitch->stichType = type;
    return stitch;
}

Row *RowSemanticAction(Sequence *elements, char * color, int isTurn) {
    printf("[BisonActions] Creating Row: color=%s, isTurn=%d, elements=%p\n",
        color ? color : "#000000", isTurn, (void*)elements);
    Row *row = calloc(1, sizeof(Row));
    row->type = ITEM_ROW;
    row->elements = elements;
    row->color = color ? strdup(color) : strdup("#000000");
    row->isTurn = isTurn;
    return row;
}

Turn *TurnSemanticAction(Sequence * chains, char *color) {
    printf("[BisonActions] Creating Turn: color=%s, chains=%p\n", color ? color : "(null)", (void*)chains);
    Turn *turn = calloc(1, sizeof(Turn));
    turn->type = ITEM_TURN;
    turn->chains = chains;
    turn->color = color ? strdup(color) : NULL;
    return turn;
}

Repeat *RepeatSemanticAction(PatternUse *pattern, int times) {
    printf("[BisonActions] Creating Repeat: times=%d, pattern=%p\n", times, (void*)pattern);
    Repeat *repeat = calloc(1, sizeof(Repeat));
    repeat->type = ITEM_REPEAT;
    repeat->pattern = pattern;
    repeat->times = times;
    return repeat;
}

Mirror *MirrorSemanticAction(PatternUse *pattern, int times) {
    printf("[BisonActions] Creating Mirror: times=%d, pattern=%p\n", times, (void*)pattern);
    Mirror *mirror = calloc(1, sizeof(Mirror));
    mirror->type = ITEM_MIRROR;
    mirror->pattern = pattern;
    mirror->times = times;
    return mirror;
}

Pattern *PatternSemanticAction(char *name, Sequence *parameters, Sequence *body) {
    printf("[BisonActions] Creating Pattern: name=%s, parameters=%p, body=%p\n",
        name ? name : "(null)", (void*)parameters, (void*)body);
    Pattern *pattern = calloc(1, sizeof(Pattern));
    pattern->type = ITEM_PATTERN;
    pattern->name = strdup(name);
    pattern->parameters = parameters;
    pattern->body = body;
    return pattern;
}

PatternUse *PatternUseSemanticAction(char *name, Sequence *arguments) {
    printf("[BisonActions] Creating PatternUse: name=%s, arguments=%p\n",
        name ? name : "(null)", (void*)arguments);
    PatternUse *use = calloc(1, sizeof(PatternUse));
    use->type = ITEM_PATTERN_USE;
    if(name) use->name = strdup(name);
    use->arguments = arguments;
    return use;
}

Sequence *SequenceSemanticAction(void *item, ItemType itemType) {
    printf("[BisonActions] Creating Sequence: itemType=%d, item=%p\n", itemType, item);
    Sequence *seq = calloc(1, sizeof(Sequence));
    seq->items = calloc(1, sizeof(void*));
    seq->items[0] = item;
    seq->count = 1;
    seq->itemTypes = calloc(1, sizeof(ItemType));
    seq->itemTypes[0] = itemType;
    return seq;
}

Sequence *AppendToSequenceSemanticAction(Sequence *seq, void *item, ItemType itemType) {
    printf("[BisonActions] Appending to Sequence: itemType=%d, newCount=%d, item=%p\n", itemType, seq ? seq->count + 1 : 1, item);
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
    printf("[BisonActions] Creating Program: declarationsAndPatterns=%p, body=%p\n", (void*)declarationsAndPatterns, (void*)body);
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
    printf("[BisonActions] Creating Constant: value=%d\n", value);
    Constant * constant = calloc(1, sizeof(Constant));
    constant->value = value;
    return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
    printf("[BisonActions] Creating ArithmeticExpression: type=%d, left=%p, right=%p\n", type, (void*)leftExpression, (void*)rightExpression);
    Expression * expression = calloc(1, sizeof(Expression));
    expression->leftExpression = leftExpression;
    expression->rightExpression = rightExpression;
    expression->type = type;
    return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
    printf("[BisonActions] Creating ConstantFactor: constant=%p\n", (void*)constant);
    Factor * factor = calloc(1, sizeof(Factor));
    factor->constant = constant;
    factor->type = CONSTANT;
    return factor;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
    printf("[BisonActions] Creating FactorExpression: factor=%p\n", (void*)factor);
    Expression * expression = calloc(1, sizeof(Expression));
    expression->factor = factor;
    expression->type = FACTOR;
    return expression;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
    printf("[BisonActions] Creating ExpressionFactor: expression=%p\n", (void*)expression);
    Factor * factor = calloc(1, sizeof(Factor));
    factor->expression = expression;
    factor->type = EXPRESSION;
    return factor;
}