%{
#include "BisonActions.h"
%}
%define api.value.union.name SemanticValue

%union {
     int integer;
    char *string;
    Stitch *stitch;
    Row *row;
    Turn *turn;
    Repeat *repeat;
    Mirror *mirror;
    Pattern *pattern;
    PatternUse *patternUse;
    ColorRow *colorRow;
    Parameter *parameter;
    Argument *argument;
    Declaration *declaration;
    Assignment *assignment;
    Sequence *sequence;
    Program *program;
    Token token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
}

%token <string> IDENTIFIER
%token <string> COLOR_VALUE
%token <integer> INTEGER
%token CH
%token SC
%token DC
%token PATTERN
%token TURN
%token MIRROR
%token REPEAT
%token COLOR
%token STITCH
%token SEMICOLON
%token COMMA
%token ASSIGNMENT
%token OPEN_BRACKET
%token CLOSE_BRACKET
%token OPEN_BRACE
%token CLOSE_BRACE
%token OPEN_PARENTHESIS
%token CLOSE_PARENTHESIS
%token UNKNOWN
%token ENDLINE
%token BEGIN_CROCHET
%token END_CROCHET

%type <parameter> parameter
%type <argument> argument
%type <declaration> declaration
%type <assignment> assignment
%type <sequence> sequence parameter_list argument_list
%type <pattern> pattern_def
%type <patternUse> pattern_use
%type <program> program
%type <row> row
%type <colorRow> color_row
%type <sequence> stitch_list
%type <turn> turn
%type <repeat> repeat
%type <mirror> mirror
%type <stitch> stitch

%%

program: sequence                                                                                                                                               { $$ = ProgramSemanticAction(currentCompilerState(),$1); }
    ;

sequence: %empty                                                                                                                                                { $$ = NULL; }
    | sequence row                                                                                                                                              { $$ = AppendToSequenceSemanticAction($1, $2, 0); }
    | sequence pattern_use                                                                                                                                      { $$ = AppendToSequenceSemanticAction($1, $2, 1); }
    | sequence pattern_def                                                                                                                                      { $$ = AppendToSequenceSemanticAction($1, $2, 2); }
    | sequence declaration                                                                                                                                      { $$ = AppendToSequenceSemanticAction($1, $2, 3); }
    | sequence assignment                                                                                                                                       { $$ = AppendToSequenceSemanticAction($1, $2, 4); }
    ;
declaration: COLOR IDENTIFIER ASSIGNMENT COLOR_VALUE SEMICOLON                                                                                                  { $$ = DeclarationSemanticAction("Color", $2, $4); }
    | STITCH IDENTIFIER ASSIGNMENT stitch SEMICOLON                                                                                                             { $$ = DeclarationSemanticAction("Stitch", $2, $4->type == STITCH_CH ? "CH" : $4->type == STITCH_SC ? "SC" : "DC"); }
    ;
assignment: IDENTIFIER ASSIGNMENT IDENTIFIER SEMICOLON                                                                                                          { $$ = AssignmentSemanticAction($1, $3); }
    ;
parameter_list: parameter                                                                                                                                       { $$ = SequenceSemanticAction($1, 0); }
    | parameter_list COMMA parameter                                                                                                                            { $$ = AppendToSequenceSemanticAction($1, $3, 0); }
    ;
parameter: COLOR IDENTIFIER                                                                                                                                     { $$ = ParameterSemanticAction(PARAM_COLOR, $2); }
    | STITCH IDENTIFIER                                                                                                                                         { $$ = ParameterSemanticAction(PARAM_STITCH, $2); }
    | PATTERN IDENTIFIER                                                                                                                                        { $$ = ParameterSemanticAction(PARAM_PATTERN, $2); }
    ;
argument_list: argument                                                                                                                                         { $$ = SequenceSemanticAction($1, 0); }
    | argument_list COMMA argument                                                                                                                              { $$ = AppendToSequenceSemanticAction($1, $3, 0); }
    ;
argument: IDENTIFIER                                                                                                                                            { $$ = ArgumentSemanticAction($1); }
    | COLOR_VALUE                                                                                                                                               { $$ = ArgumentSemanticAction($1); }
    | stitch                                                                                                                                                    { $$ = ArgumentSemanticAction($1->type == STITCH_CH ? "CH" : $1->type == STITCH_SC ? "SC" : "DC"); }
    ;
pattern_def: PATTERN IDENTIFIER OPEN_PARENTHESIS parameter_list CLOSE_PARENTHESIS OPEN_BRACE sequence CLOSE_BRACE SEMICOLON                                     { $$ = PatternSemanticAction($2, $4, $7); }
    | PATTERN IDENTIFIER OPEN_BRACE sequence CLOSE_BRACE SEMICOLON                                                                                              { $$ = PatternSemanticAction($2, NULL, $4); }
    ;
pattern_use: IDENTIFIER OPEN_PARENTHESIS argument_list CLOSE_PARENTHESIS                                                                                        { $$ = PatternUseSemanticAction($1, $3); }
    | IDENTIFIER                                                                                                                                                { $$ = PatternUseSemanticAction($1, NULL); }
    ;
row: stitch_list SEMICOLON                                                                                                                                      { $$ = RowSemanticAction($1); }
    | turn SEMICOLON                                                                                                                                            { $$ = $1; }
    | repeat SEMICOLON                                                                                                                                          { $$ = $1; }
    | mirror SEMICOLON                                                                                                                                          { $$ = $1; }
    | color_row SEMICOLON                                                                                                                                       { $$ = $1; }
    ;
color_row: COLOR_VALUE stitch_list                                                                                                                              { $$ = ColorRowSemanticAction($1, RowSemanticAction($2)); }
    ;
stitch_list: stitch                                                                                                                                             { $$ = SequenceSemanticAction($1, 0); }
    | stitch_list stitch                                                                                                                                        { $$ = AppendToSequenceSemanticAction($1, $2, 0); }
    ;
stitch: CH                                                                                                                                                      { $$ = StitchSemanticAction(STITCH_CH); }
    | SC                                                                                                                                                        { $$ = StitchSemanticAction(STITCH_SC); }
    | DC                                                                                                                                                        { $$ = StitchSemanticAction(STITCH_DC); }
    ;
repeat: REPEAT OPEN_PARENTHESIS pattern_use COMMA INTEGER CLOSE_PARENTHESIS stitch                                                                              { $$ = RepeatSemanticAction(NULL, $5, $7); }
    | REPEAT OPEN_PARENTHESIS OPEN_BRACKET stitch_list CLOSE_BRACKET COMMA INTEGER CLOSE_PARENTHESIS stitch                                                     { $$ = RepeatSemanticAction($4, $7, $9); }
    ;
mirror: MIRROR OPEN_PARENTHESIS OPEN_BRACKET stitch_list CLOSE_BRACKET COMMA INTEGER CLOSE_PARENTHESIS                                                          { $$ = MirrorSemanticAction($4, $7); }
    ;
turn: TURN stitch_list                                                                                                                                          { $$ = TurnSemanticAction($2->count, NULL); }
    | COLOR_VALUE TURN stitch_list                                                                                                                              { $$ = TurnSemanticAction($3->count, $1); }
    ;
%%
