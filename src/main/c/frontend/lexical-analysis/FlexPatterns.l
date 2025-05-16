%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x CROCHET
%x PATTERN

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

identifierName [a-zA-Z][a-zA-Z0-9_]*
integer [[:digit:]]+
endline [\n]
whitespace  [ \f\n\r\t\v]+
inlineWhitespace [ \f\r\t\v]+
color #([0-9A-F]{6}|[0-9a-f]{6})

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"Pattern"                           { BEGIN(PATTERN); return PatternLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>"}"                        { BEGIN(INITIAL); return BraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }
<PATTERN>"{"                        { return BraceLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
<PATTERN>"("                        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
<PATTERN>")"                        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
<PATTERN>"["                        { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
<PATTERN>"]"                        { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
<PATTERN>";"                        { return SemicolonLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>","                        { return CommaLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>"Color"                    { return TypeLexemeAction(createLexicalAnalyzerContext(), COLOR); }
<PATTERN>"Stitch"                   { return TypeLexemeAction(createLexicalAnalyzerContext(), STITCH); }
<PATTERN>"Pattern"                  { return TypeLexemeAction(createLexicalAnalyzerContext(), PATTERN); }
<PATTERN>"CH"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), CH); }
<PATTERN>"SC"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), SC); }
<PATTERN>"DC"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), DC); }
<PATTERN>"MIRROR"                   { return FunctionLexemeAction(createLexicalAnalyzerContext(), MIRROR); }
<PATTERN>"REPEAT"                   { return FunctionLexemeAction(createLexicalAnalyzerContext(), REPEAT); }
<PATTERN>"TURN"                     { return TurnLexemeAction(createLexicalAnalyzerContext()); }  
<PATTERN>{color}                    { return ColorLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>{integer}                  { return IntegerLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>{endline}                  {  }
<PATTERN>{inlineWhitespace}         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>{identifierName}           { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN>.                          { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

"BEGIN CROCHET"                     { BEGIN(CROCHET); return EntryPointLexemeAction(createLexicalAnalyzerContext(), BEGIN_CROCHET); }
<CROCHET>"END CROCHET"              { BEGIN(INITIAL); return EntryPointLexemeAction(createLexicalAnalyzerContext(), END_CROCHET); }
<CROCHET>"("                        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
<CROCHET>")"                        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
<CROCHET>"["                        { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
<CROCHET>"]"                        { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
<CROCHET>";"                        { return SemicolonLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>","                        { return CommaLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>"CH"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), CH); }
<CROCHET>"SC"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), SC); }
<CROCHET>"DC"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), DC); }
<CROCHET>"MIRROR"                   { return FunctionLexemeAction(createLexicalAnalyzerContext(), MIRROR); }
<CROCHET>"REPEAT"                   { return FunctionLexemeAction(createLexicalAnalyzerContext(), REPEAT); }
<CROCHET>"TURN"                     { return TurnLexemeAction(createLexicalAnalyzerContext()); }  
<CROCHET>{color}                    { return ColorLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{integer}                  { return IntegerLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{endline}                  { }
<CROCHET>{inlineWhitespace}         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{identifierName}           { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>.                          { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

{endline}                            { /* Ignorar en cualquier contexto */ }
"Stitch"                            { return TypeLexemeAction(createLexicalAnalyzerContext(), STITCH); }
"Color"                             { return TypeLexemeAction(createLexicalAnalyzerContext(), COLOR); }
"="                                 { return AssignmentLexemeAction(createLexicalAnalyzerContext()); }
";"                                 { return SemicolonLexemeAction(createLexicalAnalyzerContext()); }
"CH"                                { return StitchLexemeAction(createLexicalAnalyzerContext(), CH); }
"SC"                                { return StitchLexemeAction(createLexicalAnalyzerContext(), SC); }
"DC"                                { return StitchLexemeAction(createLexicalAnalyzerContext(), DC); }
{color}                             { return ColorLexemeAction(createLexicalAnalyzerContext()); }
{identifierName}                    { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }
{whitespace}                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
