/* Mikey Mauricio 
 lex file 
CS57 
Wk1 Project 4/9/2023
*/

%{
  #include "ast.h"
	#include <stdio.h>
	#include "y.tab.h"
  #include <string.h>
  void yyerror(const char *);
  
%}

%%

"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"extern"                { return EXTERN; }
"print"                 { return PRINT; }
"return"                { return RETURN; }
"void"                  { return VOID; }
"int"                   { return INT; }
"read"                  { return READ; }

[a-z][a-zA-Z0-9_]*      {             
                          yylval.sName = strdup(yytext);         
                          return ID;
                        }

0|[1-9][0-9]*      		  { 
                          yylval.iVal = atoi(yytext);
                          return NUM;
                        }

[-<>=+/*{}.]            { return yytext[0]; }
"("                     { return yytext[0]; }
")"                     { return yytext[0]; }
";"                     { return yytext[0]; }
"<="                    { return LE; }
">="                    { return GE; }
"=="                    { return EQ; }
"!="                    { return NE; }

[  \t\n]+          {}

%%

int yywrap(void){
	return 1;

}



