#include "BisonActions.h"
#include "../../shared/structures/ScopeListADT.h"

static ScopeListADT _scopeList = NULL;
static CompilerState *_compilerState = NULL;
static int i = 0;
/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeBisonActionsModule()
{
    _logger = createLogger("BisonActions");
    _compilerState = currentCompilerState();

    // _compilerState->scopeList = newScopeList();
    // _scopeList = _compilerState->scopeList;

    _scopeList = newScopeList();
    insertNewScope(_scopeList);
}

void shutdownBisonActionsModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }

    if (_scopeList != NULL)
    {
        freeScopeList(_scopeList);
    }
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char *functionName);
void *buildValueByVariableType(char *varValue, VariableType varType);
VariableType getVariableTypeByTypeName(char *typeName);
void * raiseCompilationError(const char * errorMsg, CompilerState * compilerState);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char *functionName)
{
    logDebugging(_logger, "%s", functionName);
}

ItemType getItemType(void *item)
{
    return *((ItemType *)item);
}

char *getStitchValue(StitchType type)
{

    char *stitch = malloc(3);
    stitch[2] = '\0';
    if (type == STITCH_CH){
        strcpy(stitch, "CH");
    }
    else if (type == STITCH_SC) {
        strcpy(stitch, "SC");
    }
    else {
        strcpy(stitch, "DC");
    }

    return stitch;
}

Parameter *ParameterSemanticAction(VariableType type, char *name)
{
    //    printf("[BisonActions] Creating Parameter: type=%d (%s), name=%s\n", type,
    //      type == PARAM_COLOR ? "Color" : type == PARAM_STITCH ? "Stitch" : "Pattern",
    //    name ? name : "(null)");
    Parameter *p = calloc(1, sizeof(Parameter));
    p->type = ITEM_PARAMETER;
    p->paramType = type;
    p->name = strdup(name);
    return p;
}

Argument *ArgumentSemanticAction(void *argumentValue, ItemType argumentType, CompilerState * compilerState)
{
    //    printf("[BisonActions] Creating Argument: type=%d\n", argumentType);
    if(argumentType == ITEM_IDENTIFIER){
        char * identifierName = (char *)argumentValue;
        VariableType types[] = {STITCH_TYPE, COLOR_TYPE, PATTERN_TYPE};
        boolean existsIdentifier = false;
        for(int i=0; i < 3 && !existsIdentifier; i++){
            existsIdentifier = existsValueByIdentifier(_scopeList, identifierName, types[i]);
        }
        if(!existsIdentifier){
            return raiseCompilationError("Identifier is never declared", compilerState);
        }
    }
    Argument *a = calloc(1, sizeof(Argument));
    a->type = ITEM_ARGUMENT;
    a->argumentType = argumentType;
    a->value = argumentValue;
    return a;
    
}

Declaration *DeclarationSemanticAction(char *typeName, char *varName, char *value, CompilerState * compilerState)
{
    //  printf("[BisonActions] Creating Declaration: type=%s, var=%s, value=%s\n",
   //     typeName ? typeName : "(null)",
   //   varName ? varName : "(null)",
   //   value ? value : "(null)");
    VariableType variableType = getVariableTypeByTypeName(typeName);
    if (existsValueByIdentifier(_scopeList, varName, variableType))
    {
        return raiseCompilationError("Variable redeclared", compilerState);
    }
    void *variableValue = buildValueByVariableType(value, variableType);
    if (variableValue == NULL)
    {
        logError(_logger, "Error: Unable to convert value %s for variable %s\n", value, varName);
        compilerState->succeed = false;
        return NULL;
    }
    putSymbolInSymbolTable(_scopeList, varName, variableType, variableValue);
    free(variableValue);

    Declaration *d = calloc(1, sizeof(Declaration));
    d->type = ITEM_DECLARATION;
    d->typeName = strdup(typeName);
    d->varName = strdup(varName);
    d->value = strdup(value);
    return d;
}

Assignment *AssignmentSemanticAction(char *varName, char *value)
{
   //    printf("[BisonActions] Creating Assignment: var=%s, value=%s\n",
    //     varName ? varName : "(null)",
    //   value ? value : "(null)");
    Assignment *a = calloc(1, sizeof(Assignment));
    a->type = ITEM_ASSIGNMENT;
    a->varName = strdup(varName);
    a->value = strdup(value);
    return a;
}

Stitch *StitchSemanticAction(StitchType type)
{
   //  printf("[BisonActions] Creating Stitch: type=%d (%s)\n", type,
     //   type == STITCH_CH ? "CH" : type == STITCH_SC ? "SC" : "DC");
    Stitch *stitch = calloc(1, sizeof(Stitch));
    stitch->type = ITEM_STITCH;
    stitch->stitchType = type;
    return stitch;
}

Row *RowSemanticAction(Sequence *elements, char *color, int isTurn)
{
   //   printf("[BisonActions] Creating Row: color=%s, isTurn=%d, elements=%p\n",
    //      color ? color : "#000000", isTurn, (void*)elements);
    Row *row = calloc(1, sizeof(Row));
    row->type = ITEM_ROW;
    row->elements = elements;
    row->color = color ? strdup(color) : strdup("#000000");
    row->isTurn = isTurn;
    return row;
}

Turn *TurnSemanticAction(Sequence *chains, char *color)
{
    //  printf("[BisonActions] Creating Turn: color=%s, chains=%p\n", color ? color : "(null)", (void*)chains);
    Turn *turn = calloc(1, sizeof(Turn));
    turn->type = ITEM_TURN;
    turn->chains = chains;
    turn->color = color ? strdup(color) : NULL;
    return turn;
}

Repeat *RepeatSemanticAction(PatternUse *pattern, int times, CompilerState * compilerState)
{
    //  printf("[BisonActions] Creating Repeat: times=%d, pattern=%p\n", times, (void*)pattern);
    if (times <=0)
    {
        return raiseCompilationError("Only positive integers must be provided in REPEAT\n", compilerState);
    }
    Repeat *repeat = calloc(1, sizeof(Repeat));
    repeat->type = ITEM_REPEAT;
    repeat->pattern = pattern;
    repeat->times = times;
    return repeat;
}

Mirror *MirrorSemanticAction(PatternUse *pattern, int times, CompilerState * compilerState)
{
    //  printf("[BisonActions] Creating Mirror: times=%d, pattern=%p\n", times, (void*)pattern);
    if (times <= 0) {
        return raiseCompilationError("Only positive integers must be provided in MIRROR\n", compilerState);
    }
    Mirror *mirror = calloc(1, sizeof(Mirror));
    mirror->type = ITEM_MIRROR;
    mirror->pattern = pattern;
    mirror->times = times;
    return mirror;
}

Pattern *PatternSemanticAction(char *name, Sequence *parameters, Sequence *body)
{
    Pattern *pattern = calloc(1, sizeof(Pattern));
    pattern->type = ITEM_PATTERN;
    pattern->name = strdup(name);
    pattern->parameters = parameters;
    pattern->body = body;
    return pattern;
}

PatternUse *PatternUseSemanticAction(char *name, Sequence *arguments, CompilerState * compilerState)
{
    //  printf("[BisonActions] Creating PatternUse: name=%s, arguments=%p\n",
    //    name ? name : "(null)", (void*)arguments);
    if(name != NULL){
        boolean patternExists = existsValueByIdentifier(_scopeList, name, PATTERN_TYPE);
        if(!patternExists){
            return raiseCompilationError("Pattern is never declared", compilerState);
        }
    }
    // Check whether the arguments are correct
    PatternData * patternData = (PatternData*)getValueByIdentifier(_scopeList, name, PATTERN_TYPE);
    if(patternData->paramCount != arguments->count){
        return raiseCompilationError("Pattern is called with wrong argument count", compilerState);
    }
    for(int i=0; i < patternData->paramCount; i++){
        Argument * currentArg = arguments->items[i];
        ItemType currentType = currentArg->argumentType;
        switch(patternData->params[i].paramType){
            case STITCH_TYPE:
                if(!(currentType == ITEM_STITCH || (currentType == ITEM_IDENTIFIER && existsValueByIdentifierAndType(_scopeList, (char *)currentArg->value, STITCH_TYPE)))){
                    return raiseCompilationError("Expected stitch as argument", compilerState);
                }
                break;
            case COLOR_TYPE:
                if(!(currentType == ITEM_COLOR_VALUE || (currentType == ITEM_IDENTIFIER && existsValueByIdentifierAndType(_scopeList, (char *)currentArg->value, COLOR_TYPE)))){
                    return raiseCompilationError("Expected color as argument", compilerState);
                }
                break;
            case PATTERN_TYPE:
                if(currentType != ITEM_PATTERN_USE && currentType != ITEM_SEQUENCE 
                    && !(currentType != ITEM_PATTERN && existsValueByIdentifierAndType(_scopeList, (char *)currentArg->value, PATTERN_TYPE))){
                    return raiseCompilationError("Expected pattern as argument", compilerState);
                }
        }
    }

    PatternUse *use = calloc(1, sizeof(PatternUse));
    use->type = ITEM_PATTERN_USE;
    if (name)
        use->name = strdup(name);
    use->arguments = arguments;
    return use;
}

Sequence *SequenceSemanticAction(void *item, ItemType itemType)
{
    //    printf("[BisonActions] Creating Sequence: itemType=%d, item=%p\n", itemType, item);
    Sequence *seq = calloc(1, sizeof(Sequence));
    seq->items = calloc(1, sizeof(void *));
    seq->items[0] = item;
    seq->count = 1;
    seq->itemTypes = calloc(1, sizeof(ItemType));
    seq->itemTypes[0] = itemType;
    return seq;
}

Sequence *AppendToSequenceSemanticAction(Sequence *seq, void *item, ItemType itemType)
{
    // printf("[BisonActions] Prepending to Sequence: itemType=%d, newCount=%d, item=%p\n", itemType, seq ? seq->count + 1 : 1, item);
    if (seq == NULL)
    {
        return SequenceSemanticAction(item, itemType);
    }
    seq->items = realloc(seq->items, sizeof(void *) * (seq->count + 1));
    seq->itemTypes = realloc(seq->itemTypes, sizeof(ItemType) * (seq->count + 1));
    // Move all existing elements one position to the right
    memmove(&seq->items[1], &seq->items[0], sizeof(void *) * seq->count);
    memmove(&seq->itemTypes[1], &seq->itemTypes[0], sizeof(ItemType) * seq->count);
    // Insert new item at the beginning
    seq->items[0] = item;
    seq->itemTypes[0] = itemType;
    seq->count++;
    return seq;
}

Program *ProgramSemanticAction(CompilerState *compilerState, Sequence *declarationsAndPatterns, Sequence *body)
{
   // printf("[BisonActions] Creating Program: declarationsAndPatterns=%p, body=%p\n", (void*)declarationsAndPatterns, (void*)body);
    Program *program = calloc(1, sizeof(Program));
    program->declarationsAndPatterns = declarationsAndPatterns;
    program->body = body;
    compilerState->abstractSyntaxtTree = program;
    if (0 < flexCurrentContext())
    {
        //   printf("[BisonActions][ERROR] The final context is not the default (0): %d\n", flexCurrentContext());
        compilerState->succeed = false;
    }else{
        compilerState->scopeList = _scopeList;
    }
    return program;
}

Constant *IntegerConstantSemanticAction(const int value)
{
    Constant *constant = calloc(1, sizeof(Constant));
    constant->value = value;
    return constant;
}

Expression *ArithmeticExpressionSemanticAction(Expression *leftExpression, Expression *rightExpression, ExpressionType type)
{
    printf("[BisonActions] Creating ArithmeticExpression: type=%d, left=%p, right=%p\n", type, (void *)leftExpression, (void *)rightExpression);
    Expression *expression = calloc(1, sizeof(Expression));
    expression->leftExpression = leftExpression;
    expression->rightExpression = rightExpression;
    expression->type = type;
    return expression;
}

Factor *ConstantFactorSemanticAction(Constant *constant)
{
    printf("[BisonActions] Creating ConstantFactor: constant=%p\n", (void *)constant);
    Factor *factor = calloc(1, sizeof(Factor));
    factor->constant = constant;
    factor->type = CONSTANT;
    return factor;
}

Expression *FactorExpressionSemanticAction(Factor *factor)
{
    printf("[BisonActions] Creating FactorExpression: factor=%p\n", (void *)factor);
    Expression *expression = calloc(1, sizeof(Expression));
    expression->factor = factor;
    expression->type = FACTOR;
    return expression;
}

Factor *ExpressionFactorSemanticAction(Expression *expression)
{
    printf("[BisonActions] Creating ExpressionFactor: expression=%p\n", (void *)expression);
    Factor *factor = calloc(1, sizeof(Factor));
    factor->expression = expression;
    factor->type = EXPRESSION;
    return factor;
}

void OpenAndFillNewScope(Sequence *parameters, CompilerState * compilerState)
{
    if (parameters == NULL) {
        return;
    }
    insertNewScope(_scopeList);
    Parameter *currentParam;
    VariableType currentParamType;
    for (int i = 0; i < parameters->count; i++)
    {
        if (parameters->itemTypes[i] == ITEM_PARAMETER)
        {
            currentParam = (Parameter *)parameters->items[i];
            if(existsValueByIdentifier(_scopeList, currentParam->name, currentParam->paramType)) {
                raiseCompilationError("Variable redeclaration inside pattern definition", compilerState);
            }
            
            putSymbolInSymbolTable(_scopeList, currentParam->name, currentParam->paramType, NULL);
        }
    }
}

void CloseLastScope(void)
{
    if (_scopeList == NULL) {
        return;
    }
    removeLastScope(_scopeList);
}

void SavePatternToCurrentScope(char *patternName, Sequence *parameters, CompilerState *compilerState)
{
    boolean patternAlreadyExists  = existsValueByIdentifier(_scopeList, patternName, PATTERN_TYPE);
    if (patternAlreadyExists)
    {
        raiseCompilationError("Pattern redeclared", compilerState);
    }

    PatternData *patternData = calloc(1, sizeof(PatternData));

    if(parameters == NULL){
        patternData->paramCount = 0;
        patternData->params = NULL;
    } else {
        patternData->paramCount = parameters->count;
        patternData->params = calloc(parameters->count, sizeof(PatternParam));
        VariableType currentParamType;
        Parameter *currentParam;
        for (int i = 0; i < parameters->count; i++)
        {
            if (parameters->itemTypes[i] == ITEM_PARAMETER)
            {
                currentParam = (Parameter *)parameters->items[i];
                patternData->params[i].paramType = currentParam->paramType;
            }
        }
    }

    putSymbolInSymbolTable(_scopeList, patternName, PATTERN_TYPE, patternData);

    free(patternData->params);
    free(patternData);
}

VariableType getVariableTypeByTypeName(char *typeName)
{
    if (strcasecmp(typeName, COLOR_NAME) == 0) {
        return COLOR_TYPE;
    }
    else if (strcasecmp(typeName, STITCH_NAME) == 0)  {
        return STITCH_TYPE;
    }
    return PATTERN_TYPE;
}

void *buildValueByVariableType(char *varValue, VariableType varType)
{
    switch (varType) {
    case STITCH_TYPE:
        StitchType *newStitchType = malloc(sizeof(StitchType));
        if (newStitchType == NULL)
        {
            return NULL;
        }
        *newStitchType = strcasecmp(varValue, "CH") == 0 ? STITCH_CH : (strcasecmp(varValue, "SC") == 0 ? STITCH_SC : STITCH_DC);
        return (void *)newStitchType;
    case COLOR_TYPE:
        char *colorName = malloc(sizeof(char) * 8);
        if (colorName == NULL)
        {
            return NULL;
        }
        strncpy(colorName, varValue, 7);
        colorName[7] = '\0';
        return (void *)colorName;
    }
}

void * raiseCompilationError(const char * errorMsg, CompilerState * compilerState){
    yyerror(errorMsg);
    compilerState->succeed = false;
    return NULL;
}