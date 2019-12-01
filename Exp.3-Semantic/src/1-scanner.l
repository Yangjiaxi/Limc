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
    #include "0-driver.h"
    #include "0-color.h"

    #if __cplusplus > 199711L
    #define register      // Deprecated in C++11.
    #endif  // #if __cplusplus > 199711L

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
    #if __cplusplus > 199711L
    #define register      // Deprecated in C++11.
    #endif  // #if __cplusplus > 199711L
    
    auto loc = &driver.get_loc();
    
    #define ACTION(NAME) Limc::Parser::make_##NAME(*loc)
    #define ACTION_ARG(NAME) Limc::Parser::make_##NAME(yytext, *loc)
%}

"/*"                    { BEGIN(COMMENT_BLOCK); }
<COMMENT_BLOCK>"*/"     { BEGIN(INITIAL); }
<COMMENT_BLOCK>\n       { driver.inc_y(1); }
<COMMENT_BLOCK>.        { }
<COMMENT_BLOCK><<EOF>>  { }

\/\/(.*)    {  }

{INTEGER}       { return ACTION_ARG(LITERAL_INTEGER); }
{FLOAT}         { return ACTION_ARG(LITERAL_FLOAT); }
{CHAR}          { return ACTION_ARG(LITERAL_CHAR); }
{STRING}        { return ACTION_ARG(LITERAL_STRING); }

"int"|"char"|"float"|"void" { return ACTION_ARG(KW_TYPE); }

"return"    { return ACTION(KW_RETURN); }
"break"     { return ACTION(KW_BREAK); }
"continue"  { return ACTION(KW_CONTINUE); }
"for"       { return ACTION(KW_FOR); }
"if"        { return ACTION(KW_IF); }
"else"      { return ACTION(KW_ELSE); }
"while"     { return ACTION(KW_WHILE); }
"do"        { return ACTION(KW_DO); }
"switch"    { return ACTION(KW_SWITCH); }
"case"      { return ACTION(KW_CASE); }
"default"   { return ACTION(KW_DEFAULT); }

{IDENTIFIER}    { return ACTION_ARG(IDENTIFIER); }

"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=" { return ACTION_ARG(OP_COMPOUND_ASSIGNMENT); }

"++"    { return ACTION_ARG(OP_INCREMENT); }
"--"    { return ACTION_ARG(OP_DECREMENT); }

">"|"<"|">="|"<="|"=="|"!=" { return ACTION_ARG(OP_RELATIONAL); }

"="     { return ACTION_ARG(OP_ASSIGNMENT);  }

">>"    { return ACTION_ARG(OP_SHIFT_RIGHT); }
"<<"    { return ACTION_ARG(OP_SHIFT_LEFT); }
"+"     { return ACTION_ARG(OP_PLUS);  }
"-"     { return ACTION_ARG(OP_MINUS);  }
"*"     { return ACTION_ARG(OP_MULTIPLY);  }
"/"     { return ACTION_ARG(OP_DIVIDE);  }
"%"     { return ACTION_ARG(OP_MODULUS);  }
"&&"    { return ACTION_ARG(OP_LOGICAL_AND); }
"&"     { return ACTION_ARG(OP_BITWISE_AND); }
"||"    { return ACTION_ARG(OP_LOGICAL_OR); }
"|"     { return ACTION_ARG(OP_BITWISE_OR); }
"^"     { return ACTION_ARG(OP_BITWISE_XOR); }
"~"     { return ACTION_ARG(OP_BITWISE_NOT); }
"!"     { return ACTION_ARG(OP_LOGICAL_NOT); }

"?"     { return ACTION(DELIM_QUESTION); }
":"     { return ACTION(DELIM_COLON); }
"("     { return ACTION(DELIM_PARENTHESIS_LEFT); }
")"     { return ACTION(DELIM_PARENTHESIS_RIGHT); }
"{"     { return ACTION(DELIM_BRACE_LEFT); }
"}"     { return ACTION(DELIM_BRACE_RIGHT); }
"["     { return ACTION(DELIM_BRACKET_LEFT); }
"]"     { return ACTION(DELIM_BRACKET_RIGHT); }
";"     { return ACTION(DELIM_SEMICOLON); }
","     { return ACTION(DELIM_COMMA); }

{NEWLINE}       { driver.inc_y(1); }
{WHITESPACE}    { }
<<EOF>>         { return ACTION(END); }

. {
    cout << RED << "Error: unexpected " << yytext << endl 
         << YELLOW << "Location: " << *loc << RESET_COLOR << endl;
}
