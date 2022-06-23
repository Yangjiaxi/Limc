LETTER		[a-zA-Z_]
DIGIT		[0-9]

INTEGER		{DIGIT}+[lLuU]?
FLOAT		({DIGIT}*\.{DIGIT}+)|({DIGIT}+\.)[lLuU]?
CHAR		'([^'\\\n]|\\.){0,1}'
STRING		\"(\\.|[^"\\])*\"
IDENTIFIER	{LETTER}({DIGIT}|{LETTER})*

NEWLINE     \n|(\r\n)
WHITESPACE  [ \t\v\f]

%{
    #include "color.h"
	int n_chars = 0, n_words = 0, n_lines = 0;
	int current_line = 1;
	#define YY_USER_ACTION n_words++; n_chars += yyleng;

	#define ACTION(NAME) printf("[" BOLD_CYAN "%3d" RESET_COLOR "]" RED " | " RESET_COLOR BLUE #NAME RESET_COLOR"\n", current_line);
    #define ACTION_ARG(NAME) printf("[" BOLD_CYAN "%3d" RESET_COLOR "]" RED " | " RESET_COLOR BLUE #NAME RESET_COLOR " [" BOLD_GREEN "%s" RESET_COLOR"]\n" RESET_COLOR, current_line, yytext);
	
%}

%option nodefault
%option noyywrap

%x COMMENT_BLOCK
%x COMMENT_LINE

%%

"/*"                    { BEGIN(COMMENT_BLOCK); }
<COMMENT_BLOCK>"*/"     { BEGIN(INITIAL); }
<COMMENT_BLOCK>\n       { current_line++; n_lines++; }
<COMMENT_BLOCK>.        { }
<COMMENT_BLOCK><<EOF>>  { }

\/\/(.*)    {  }

{INTEGER}       { ACTION_ARG(LITERAL_INTEGER); }
{FLOAT}         { ACTION_ARG(LITERAL_FLOAT); }
{CHAR}          { ACTION_ARG(LITERAL_CHAR); }
{STRING}        { ACTION_ARG(LITERAL_STRING); }

"int"|"char"|"float"|"void" { ACTION_ARG(KW_TYPE); }

"return"    { ACTION(KW_RETURN); }
"break"     { ACTION(KW_BREAK); }
"continue"  { ACTION(KW_CONTINUE); }
"for"       { ACTION(KW_FOR); }
"if"        { ACTION(KW_IF); }
"else"      { ACTION(KW_ELSE); }
"while"     { ACTION(KW_WHILE); }
"do"        { ACTION(KW_DO); }
"struct"    { ACTION(KW_STRUCT); }
"use"       { ACTION(KW_USE); }

{IDENTIFIER}    { ACTION_ARG(IDENTIFIER); }

"+="|"-="|"*="|"/="|"%="    { ACTION_ARG(OP_ARITH_COMPOUND_ASSIGNMENT); }
"&="|"|="|"^="|"<<="|">>="  { ACTION_ARG(OP_LOG_COMPOUND_ASSIGNMENT); }

"++"    { ACTION_ARG(OP_INCREMENT); }
"--"    { ACTION_ARG(OP_DECREMENT); }

">"|"<"|">="|"<="|"=="|"!=" { ACTION_ARG(OP_RELATIONAL); }

"="     { ACTION_ARG(OP_ASSIGNMENT);  }

">>"    { ACTION_ARG(OP_SHIFT_RIGHT); }
"<<"    { ACTION_ARG(OP_SHIFT_LEFT); }
"+"     { ACTION_ARG(OP_PLUS);  }
"-"     { ACTION_ARG(OP_MINUS);  }
"*"     { ACTION_ARG(OP_MULTIPLY);  }
"/"     { ACTION_ARG(OP_DIVIDE);  }
"%"     { ACTION_ARG(OP_MODULUS);  }
"&&"    { ACTION_ARG(OP_LOGICAL_AND); }
"&"     { ACTION_ARG(OP_BITWISE_AND); }
"||"    { ACTION_ARG(OP_LOGICAL_OR); }
"|"     { ACTION_ARG(OP_BITWISE_OR); }
"^"     { ACTION_ARG(OP_BITWISE_XOR); }
"~"     { ACTION_ARG(OP_BITWISE_NOT); }
"!"     { ACTION_ARG(OP_LOGICAL_NOT); }
"."     { ACTION_ARG(OP_DOT); }

"?"     { ACTION(DELIM_QUESTION); }
":"     { ACTION(DELIM_COLON); }
"("     { ACTION(DELIM_PARENTHESIS_LEFT); }
")"     { ACTION(DELIM_PARENTHESIS_RIGHT); }
"{"     { ACTION(DELIM_BRACE_LEFT); }
"}"     { ACTION(DELIM_BRACE_RIGHT); }
"["     { ACTION(DELIM_BRACKET_LEFT); }
"]"     { ACTION(DELIM_BRACKET_RIGHT); }
";"     { ACTION(DELIM_SEMICOLON); }
","     { ACTION(DELIM_COMMA); }

{NEWLINE}       { current_line++; n_lines++; }
{WHITESPACE}    {}

.	{ ACTION_ARG(ERROR); }

%%

int main()
{
	yylex();
	printf("\t# chars: %d\n", n_chars);
	printf("\t# words: %d\n", n_words);
	printf("\t# lines: %d\n", n_lines);
	return 0;
}
