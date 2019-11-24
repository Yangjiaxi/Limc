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
    #include "driver.h"
    #include "color.h"
    #define YY_USER_ACTION driver.inc_x(yyleng);
%}
%option nodefault
%option noyywrap
%option c++

%option yyclass="Scanner"
%option prefix="Limc_"

%x COMMENT_BLOCK

%%
%{
    auto loc = driver.getLocation();
    #define ACTION(NAME) Limc::Parser::make_##NAME(loc)
    #define ACTION_ARG(NAME) Limc::Parser::make_##NAME(yytext, loc)
%}

"/*"                    { BEGIN(COMMENT_BLOCK); }
<COMMENT_BLOCK>"*/"     { BEGIN(INITIAL); }
<COMMENT_BLOCK>\n       { driver.inc_y(1); }
<COMMENT_BLOCK>.        { }
<COMMENT_BLOCK><<EOF>>  { }

\/\/(.*)    { driver.inc_y(1); }

{INTEGER}       { return ACTION_ARG(LITERAL_INTEGER); }
{FLOAT}         { return ACTION_ARG(LITERAL_FLOAT); }
{CHAR}          { return ACTION_ARG(LITERAL_CHAR); }
{STRING}        { return ACTION_ARG(LITERAL_STRING); }

"int"|"float"|"long"|"char"|"double"|"void"|"short" { return ACTION_ARG(KEYWORD_TYPE); }

"return"    { return ACTION(KEYWORD_RETURN); }
"break"     { return ACTION(KEYWORD_BREAK); }
"continue"  { return ACTION(KEYWORD_CONTINUE); }
"for"       { return ACTION(KEYWORD_FOR); }
"if"        { return ACTION(KEYWORD_IF); }
"else"      { return ACTION(KEYWORD_ELSE); }
"while"     { return ACTION(KEYWORD_WHILE); }
"do"        { return ACTION(KEYWORD_DO); }
"switch"    { return ACTION(KEYWORD_SWITCH); }
"case"      { return ACTION(KEYWORD_CASE); }
"default"   { return ACTION(KEYWORD_DEFAULT); }

{IDENTIFIER}    { return ACTION_ARG(IDENTIFIER); }

"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=" { return ACTION_ARG(OPERATOR_COMPOUND_ASSIGNMENT); }

"++"    { return ACTION_ARG(OPERATOR_INCREMENT); }
"--"    { return ACTION_ARG(OPERATOR_DECREMENT); }

">"|"<"|">="|"<="|"=="|"!=" { return ACTION_ARG(OPERATOR_RELATIONAL); }

"="     { return ACTION_ARG(OPERATOR_ASSIGNMENT);  }
">>"    { return ACTION_ARG(OPERATOR_SHIFT_RIGHT); }
"<<"    { return ACTION_ARG(OPERATOR_SHIFT_LEFT); }
"+"     { return ACTION_ARG(OPERATOR_PLUS);  }
"-"     { return ACTION_ARG(OPERATOR_MINUS);  }
"*"     { return ACTION_ARG(OPERATOR_MULTIPLY);  }
"/"     { return ACTION_ARG(OPERATOR_DIVIDE);  }
"%"     { return ACTION_ARG(OPERATOR_MODULUS);  }
"&&"    { return ACTION_ARG(OPERATOR_LOGICAL_AND); }
"&"     { return ACTION_ARG(OPERATOR_BITWISE_AND); }
"||"    { return ACTION_ARG(OPERATOR_LOGICAL_OR); }
"|"     { return ACTION_ARG(OPERATOR_BITWISE_OR); }
"^"     { return ACTION_ARG(OPERATOR_BITWISE_XOR); }
"~"     { return ACTION_ARG(OPERATOR_BITWISE_NOT); }
"!"     { return ACTION_ARG(OPERATOR_LOGICAL_NOT); }

"?"     { return ACTION(OPERATOR_QUESTION); }
":"     { return ACTION(PUNCTUATOR_COLON); }
"("     { return ACTION(PUNCTUATOR_PARENTHESIS_LEFT); }
")"     { return ACTION(PUNCTUATOR_PARENTHESIS_RIGHT); }
"{"     { return ACTION(PUNCTUATOR_BRACE_LEFT); }
"}"     { return ACTION(PUNCTUATOR_BRACE_RIGHT); }
"["     { return ACTION(PUNCTUATOR_BRACKET_LEFT); }
"]"     { return ACTION(PUNCTUATOR_BRACKET_RIGHT); }
";"     { return ACTION(PUNCTUATOR_SEMICOLON); }
","     { return ACTION(PUNCTUATOR_COMMA); }

{NEWLINE}       { driver.inc_y(1); }
{WHITESPACE}    { }
<<EOF>>         { return ACTION(END); }

. {
    cout << RED << "Error: unexpected " << yytext << endl 
         << YELLOW << "Location: " << loc << RESET_COLOR << endl;
}
