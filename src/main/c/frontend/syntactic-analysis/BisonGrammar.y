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
	void * row_element;
    Token token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
}

%destructor { releaseStitch($$); } <stitch>
%destructor { releaseRow($$); } <row>
%destructor { releaseRepeat($$); } <repeat>
%destructor { releaseMirror($$); } <mirror>
%destructor { releasePattern($$); } <pattern>
%destructor { releasePatternUse($$); } <patternUse>
%destructor { releaseParameter($$); } <parameter>
%destructor { releaseArgument($$); } <argument>
%destructor { releaseDeclaration($$); } <declaration>
%destructor { releaseAssignment($$); } <assignment>
%destructor { releaseSequence($$); } <sequence>
%destructor { free($$); } <string>

%token <string> IDENTIFIER
%token <string> COLOR_VALUE
%token <integer> INTEGER
%token <token> CH
%token <token> SC
%token <token> DC
%token <token> PATTERN
%token <token> TURN
%token <token> MIRROR
%token <token> REPEAT
%token <token> COLOR
%token <token> STITCH
%token <token> SEMICOLON
%token <token> COMMA
%token <token> ASSIGNMENT
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> UNKNOWN
%token <token> ENDLINE
%token <token> BEGIN_CROCHET
%token <token> END_CROCHET

%type <parameter> parameter
%type <argument> argument
%type <declaration> declaration
%type <assignment> assignment
%type <sequence> sequence parameter_list argument_list
%type <pattern> pattern_def
%type <patternUse> pattern_use
%type <patternUse> pattern_use_in_function
%type <program> program
%type <row> row
%type <sequence> row_elements
%type <row_element> row_element 
%type <sequence> stitch_list
%type <repeat> repeat
%type <mirror> mirror
%type <stitch> stitch
%type <sequence> declarations_and_patterns
%nonassoc SHIFT_PRECEDENCE
%nonassoc REDUCE_PRECEDENCE

%%

program
    : declarations_and_patterns BEGIN_CROCHET sequence END_CROCHET      { $$ = ProgramSemanticAction(currentCompilerState(), $1, $3); }
    ;

declarations_and_patterns: %empty                                       { $$ = NULL; }
    | declaration declarations_and_patterns                             { $$ = AppendToSequenceSemanticAction($2, $1, ITEM_DECLARATION); }
    | assignment declarations_and_patterns                              { $$ = AppendToSequenceSemanticAction($2, $1, ITEM_ASSIGNMENT); }
    | pattern_def declarations_and_patterns                             { $$ = AppendToSequenceSemanticAction($2, $1, ITEM_PATTERN); }
    ;

sequence: %empty														{ $$ = NULL; }
    | row sequence 														{ $$ = AppendToSequenceSemanticAction($2, $1, ITEM_ROW); }
    | pattern_use  SEMICOLON sequence 									{ $$ = AppendToSequenceSemanticAction($3, $1, ITEM_PATTERN_USE); }
    | pattern_def sequence 												{ $$ = AppendToSequenceSemanticAction($2, $1, ITEM_PATTERN); }
    | declaration sequence 												{ $$ = AppendToSequenceSemanticAction($2, $1, ITEM_DECLARATION); }
    | assignment sequence												{ $$ = AppendToSequenceSemanticAction($2, $1, ITEM_ASSIGNMENT); }
    ;

declaration: COLOR IDENTIFIER ASSIGNMENT COLOR_VALUE SEMICOLON			{ $$ = DeclarationSemanticAction(COLOR_NAME, $2, $4); free($2); free($4);}
    | STITCH IDENTIFIER ASSIGNMENT stitch SEMICOLON						{ char * stitch = getStitchValue($4->type); $$ = DeclarationSemanticAction(STITCH_NAME, $2, stitch); free(stitch); free($2);releaseStitch($4); }
    ;

assignment: IDENTIFIER ASSIGNMENT IDENTIFIER SEMICOLON					{ $$ = AssignmentSemanticAction($1, $3); free($1); free($3);}
    ;

parameter_list: parameter												{ $$ = SequenceSemanticAction($1, ITEM_PARAMETER); }
    | parameter COMMA parameter_list									{ $$ = AppendToSequenceSemanticAction($3, $1, ITEM_PARAMETER); }
    ;

parameter: COLOR IDENTIFIER												{ $$ = ParameterSemanticAction(PARAM_COLOR, $2); free($2);}
    | STITCH IDENTIFIER													{ $$ = ParameterSemanticAction(PARAM_STITCH, $2);free($2); }
    | PATTERN IDENTIFIER											    { $$ = ParameterSemanticAction(PARAM_PATTERN, $2); free($2);}
    
argument_list: argument													{ $$ = SequenceSemanticAction($1, ITEM_ARGUMENT); }
    | argument COMMA argument_list										{ $$ = AppendToSequenceSemanticAction($3, $1, ITEM_ARGUMENT); }
    ;

argument: IDENTIFIER													{ $$ = ArgumentSemanticAction($1, ITEM_IDENTIFIER); }
    | COLOR_VALUE														{ $$ = ArgumentSemanticAction($1, ITEM_COLOR_VALUE); }
    | stitch															{ $$ = ArgumentSemanticAction($1, ITEM_STITCH); }
    | pattern_use                                                       { $$ = ArgumentSemanticAction($1, ITEM_PATTERN_USE); }
    | OPEN_BRACKET stitch_list CLOSE_BRACKET                            { $$ = ArgumentSemanticAction($2, ITEM_SEQUENCE); }
    ;

pattern_def: PATTERN IDENTIFIER OPEN_PARENTHESIS parameter_list CLOSE_PARENTHESIS OPEN_BRACE sequence CLOSE_BRACE SEMICOLON                                     { $$ = PatternSemanticAction($2, $4, $7);free($2);  }
            | PATTERN IDENTIFIER OPEN_PARENTHESIS CLOSE_PARENTHESIS OPEN_BRACE sequence CLOSE_BRACE SEMICOLON                                                           { $$ = PatternSemanticAction($2, NULL, $6);free($2);  }
            ;

pattern_use: IDENTIFIER OPEN_PARENTHESIS argument_list CLOSE_PARENTHESIS    { $$ = PatternUseSemanticAction($1, $3); free($1); }
           | IDENTIFIER OPEN_PARENTHESIS CLOSE_PARENTHESIS                  { $$ = PatternUseSemanticAction($1, NULL);free($1);  }
           ;

pattern_use_in_function: pattern_use                                    { $$ = $1; }
            |   IDENTIFIER                                              { $$ = PatternUseSemanticAction($1, NULL); free($1); }
            ;

row_element: stitch                                                     { $$ = $1; }
           | repeat                                                     { $$ = $1; }
           | mirror                                                     { $$ = $1; }
           | IDENTIFIER                                                 { $$ = ArgumentSemanticAction($1, ITEM_IDENTIFIER); }
            ;

row_elements: row_element                                               { $$ = SequenceSemanticAction($1, getItemType($1)); }
            | row_elements row_element                                  { $$ = AppendToSequenceSemanticAction($1, $2, getItemType($2)); }

row: row_elements SEMICOLON												{ $$ = RowSemanticAction($1, NULL, ISNOTTURN); }
	|	COLOR_VALUE row_elements SEMICOLON								{ $$ = RowSemanticAction($2, $1, ISNOTTURN);free($1); }
	|	TURN row_elements SEMICOLON										{ $$ = RowSemanticAction($2, NULL, ISTURN); }
	|	COLOR_VALUE TURN row_elements SEMICOLON							{ $$ = RowSemanticAction($3, $1, ISTURN); free($1);}
    ;

stitch_list: stitch	%prec REDUCE_PRECEDENCE								{ $$ = SequenceSemanticAction($1, ITEM_STITCH); }
    | stitch_list stitch %prec SHIFT_PRECEDENCE							{ $$ = AppendToSequenceSemanticAction($1, $2, ITEM_STITCH); }
    ;

stitch: CH																{ $$ = StitchSemanticAction(STITCH_CH); }
    | SC																{ $$ = StitchSemanticAction(STITCH_SC); }
    | DC																{ $$ = StitchSemanticAction(STITCH_DC); }
    ;

repeat: REPEAT OPEN_PARENTHESIS pattern_use_in_function COMMA INTEGER CLOSE_PARENTHESIS								{ $$ = RepeatSemanticAction($3, $5); }
    | REPEAT OPEN_PARENTHESIS OPEN_BRACKET stitch_list CLOSE_BRACKET COMMA INTEGER CLOSE_PARENTHESIS                { PatternUse *anon = PatternUseSemanticAction(NULL, $4);  $$ = RepeatSemanticAction(anon, $7);}

    ;

mirror: MIRROR OPEN_PARENTHESIS pattern_use_in_function COMMA INTEGER CLOSE_PARENTHESIS								{ $$ = MirrorSemanticAction($3, $5); }
    | MIRROR OPEN_PARENTHESIS OPEN_BRACKET stitch_list CLOSE_BRACKET COMMA INTEGER CLOSE_PARENTHESIS                { PatternUse *anon = PatternUseSemanticAction(NULL, $4);  $$ = MirrorSemanticAction(anon, $7); }
    ;

%%
