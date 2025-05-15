#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

Token PatternLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token EntryPointLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token ParenthesisLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token BraceLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token BracketLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token TypeLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token StitchLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token FunctionLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token EndlineLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token IdentifierLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token IntegerLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token SemicolonLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token CommaLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token ColorLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token TurnLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token AssignmentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

LexicalAnalyzerContext* createLexicalAnalyzerContext();

Token UnknownLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

#endif
