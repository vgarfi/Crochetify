#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/Logger.h"
#include "AbstractSyntaxTree.h"
#include <stdlib.h>
#include "../../shared/CompilerState.h"
#include "../../shared/Type.h"
#include "SyntacticAnalyzer.h"

/* Inicialización y destrucción del módulo */
void initializeBisonActionsModule();
void shutdownBisonActionsModule();

/* Acciones semánticas para cada tipo de nodo */
Stitch *StitchSemanticAction(StitchType type);
Row *RowSemanticAction(Sequence *elements, char * color, int isTurn);
Turn *TurnSemanticAction(Sequence * chains, char *color);
Repeat *RepeatSemanticAction(Sequence *pattern, int times);
Repeat *AnonymousRepeatSemanticAction(PatternUse *anon);
Mirror *MirrorSemanticAction(Sequence *pattern, int times);
Mirror *AnonymousMirrorSemanticAction(PatternUse *anon);
Pattern *PatternSemanticAction(char *name, Sequence *parameters, Sequence *body);
PatternUse *PatternUseSemanticAction(char *name, Sequence *arguments);
ColorRow *ColorRowSemanticAction(char *color, Row *row);
Parameter *ParameterSemanticAction(ParameterType type, char *name);
Argument *ArgumentSemanticAction(char *name);
Argument *AnonymousArgumentSemanticAction(PatternUse *anon);
Declaration *DeclarationSemanticAction(char *typeName, char *varName, char *value);
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

#endif