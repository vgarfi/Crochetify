#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/Logger.h"
#include "AbstractSyntaxTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "../../shared/CompilerState.h"
#include "../../shared/Type.h"
#include "SyntacticAnalyzer.h"
#include "../../shared/structures/RowNodeListADT.h"
#include "../../shared/structures/SymbolTable.h"
#include "../../shared/structures/ScopeListADT.h"

/* Inicialización y destrucción del módulo */
void initializeBisonActionsModule();
void shutdownBisonActionsModule();

/* Acciones semánticas para cada tipo de nodo */
Stitch *StitchSemanticAction(StitchType type);
char * getStitchValue(StitchType type);
Row *RowSemanticAction(Sequence *elements, char * color, int isTurn);
Turn *TurnSemanticAction(Sequence * chains, char *color);
Repeat *RepeatSemanticAction(PatternUse *pattern, int times);
Mirror *MirrorSemanticAction(PatternUse *pattern, int times);
Pattern *PatternSemanticAction(char *name, Sequence *parameters, Sequence *body);
PatternUse *PatternUseSemanticAction(char *name, Sequence *arguments, CompilerState * compilerState);
Parameter *ParameterSemanticAction(VariableType type, char *name);
Argument *ArgumentSemanticAction(void *argumentValue, ItemType argumentType, CompilerState * compilerState);
Declaration *DeclarationSemanticAction(char *typeName, char *varName, char *value, CompilerState * compilerState);
Assignment *AssignmentSemanticAction(char *varName, char *value);
Sequence *SequenceSemanticAction(void *item, ItemType itemType);
Sequence *AppendToSequenceSemanticAction(Sequence *seq, void *item, ItemType itemType);
Program *ProgramSemanticAction(CompilerState * compilerState, Sequence *declarationsAndPatterns, Sequence *body);
ItemType getItemType(void *item);
Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression);

// Semantic Analysis Functions for scopes
void OpenAndFillNewScope(Sequence *parameters);
void CloseLastScope(void);
void SavePatternToCurrentScope(char* patternName, Sequence *parameters, CompilerState * compilerState);

#endif