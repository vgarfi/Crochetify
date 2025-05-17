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
%x INLINE_COMMENT
%x CROCHET
%x PATTERN_DEFINITION

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

"//"                                           { yy_push_state(INLINE_COMMENT); BeginInlineCommentLexemeAction(createLexicalAnalyzerContext()); }
<INLINE_COMMENT>{endline}				       { EndInlineCommentLexemeAction(createLexicalAnalyzerContext());  yy_pop_state(); }
<INLINE_COMMENT>.*				               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"/*"                                           { yy_push_state(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				           { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); yy_pop_state(); }
<MULTILINE_COMMENT>[[:space:]]+		           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"Pattern"                                      { BEGIN(PATTERN_DEFINITION); return PatternLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>"}"                        { BEGIN(INITIAL); return BraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }
<PATTERN_DEFINITION>"//"                       { yy_push_state(INLINE_COMMENT); BeginInlineCommentLexemeAction(createLexicalAnalyzerContext()); }

<PATTERN_DEFINITION>"{"                        { return BraceLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
<PATTERN_DEFINITION>"("                        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
<PATTERN_DEFINITION>")"                        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
<PATTERN_DEFINITION>"["                        { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
<PATTERN_DEFINITION>"]"                        { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
<PATTERN_DEFINITION>";"                        { return SemicolonLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>","                        { return CommaLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>"Color"                    { return TypeLexemeAction(createLexicalAnalyzerContext(), COLOR); }
<PATTERN_DEFINITION>"Stitch"                   { return TypeLexemeAction(createLexicalAnalyzerContext(), STITCH); }
<PATTERN_DEFINITION>"Pattern"                  { return TypeLexemeAction(createLexicalAnalyzerContext(), PATTERN); }
<PATTERN_DEFINITION>"CH"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), CH); }
<PATTERN_DEFINITION>"SC"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), SC); }
<PATTERN_DEFINITION>"DC"                       { return StitchLexemeAction(createLexicalAnalyzerContext(), DC); }
<PATTERN_DEFINITION>"MIRROR"                   { return FunctionLexemeAction(createLexicalAnalyzerContext(), MIRROR); }
<PATTERN_DEFINITION>"REPEAT"                   { return FunctionLexemeAction(createLexicalAnalyzerContext(), REPEAT); }
<PATTERN_DEFINITION>"TURN"                     { return TurnLexemeAction(createLexicalAnalyzerContext()); }  
<PATTERN_DEFINITION>{color}                    { return ColorLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>{integer}                  { return IntegerLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>{endline}                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>{inlineWhitespace}         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>{identifierName}           { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }
<PATTERN_DEFINITION>.                          { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

"BEGIN CROCHET"                                { BEGIN(CROCHET); return EntryPointLexemeAction(createLexicalAnalyzerContext(), BEGIN_CROCHET); }
<CROCHET>"END CROCHET"                         { BEGIN(INITIAL); return EntryPointLexemeAction(createLexicalAnalyzerContext(), END_CROCHET); }
<CROCHET>"//"                                  { yy_push_state(INLINE_COMMENT); BeginInlineCommentLexemeAction(createLexicalAnalyzerContext()); }

<CROCHET>"("                                   { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
<CROCHET>")"                                   { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
<CROCHET>"["                                   { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
<CROCHET>"]"                                   { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
<CROCHET>";"                                   { return SemicolonLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>","                                   { return CommaLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>"CH"                                  { return StitchLexemeAction(createLexicalAnalyzerContext(), CH); }
<CROCHET>"SC"                                  { return StitchLexemeAction(createLexicalAnalyzerContext(), SC); }
<CROCHET>"DC"                                  { return StitchLexemeAction(createLexicalAnalyzerContext(), DC); }
<CROCHET>"MIRROR"                              { return FunctionLexemeAction(createLexicalAnalyzerContext(), MIRROR); }
<CROCHET>"REPEAT"                              { return FunctionLexemeAction(createLexicalAnalyzerContext(), REPEAT); }
<CROCHET>"TURN"                                { return TurnLexemeAction(createLexicalAnalyzerContext()); }  
<CROCHET>{color}                               { return ColorLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{integer}                             { return IntegerLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{endline}                             { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{inlineWhitespace}                    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>{identifierName}                      { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }
<CROCHET>.                                     { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

{endline}                                      { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Stitch"                                       { return TypeLexemeAction(createLexicalAnalyzerContext(), STITCH); }
"Color"                                        { return TypeLexemeAction(createLexicalAnalyzerContext(), COLOR); }
"="                                            { return AssignmentLexemeAction(createLexicalAnalyzerContext()); }
";"                                            { return SemicolonLexemeAction(createLexicalAnalyzerContext()); }
"CH"                                           { return StitchLexemeAction(createLexicalAnalyzerContext(), CH); }
"SC"                                           { return StitchLexemeAction(createLexicalAnalyzerContext(), SC); }
"DC"                                           { return StitchLexemeAction(createLexicalAnalyzerContext(), DC); }
{color}                                        { return ColorLexemeAction(createLexicalAnalyzerContext()); }
{identifierName}                               { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }
{whitespace}                                   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									           { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
