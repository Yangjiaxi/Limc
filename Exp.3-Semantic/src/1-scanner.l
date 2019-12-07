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
    
    #define DEBUG 0
    #define ACTION(NAME) if(DEBUG){cout<<#NAME << endl;} return Limc::Parser::make_##NAME(*loc)
    #define ACTION_ARG(NAME) if(DEBUG){cout<<#NAME << endl;} return Limc::Parser::make_##NAME(yytext, *loc)
%}

"/*"                    { BEGIN(COMMENT_BLOCK); }
<COMMENT_BLOCK>"*/"     { BEGIN(INITIAL); }
<COMMENT_BLOCK>\n       { driver.inc_y(1); }
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

{IDENTIFIER}    { ACTION_ARG(IDENTIFIER); }

"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=" { ACTION_ARG(OP_COMPOUND_ASSIGNMENT); }

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

{NEWLINE}       { driver.inc_y(1); }
{WHITESPACE}    { }
<<EOF>>         { ACTION(END); }

. {
    cout << RED << "Error: unexpected " << yytext << endl 
         << YELLOW << "Location: " << *loc << RESET_COLOR << endl;
}
