/* Mikey Mauricio 
 lex file 
CS57 
Wk1 Project 4/9/2023
*/

%{
	#include <stdio.h>
	#include "y.tab.h"
  extern int yylval;
  void yyerror(char *);
%}

/* for digits */
digit [1-9][0-9]+

/* for letters */ 
letter [a-z]


%%

"if"              { printf("%s", yytext); return IF; }
"else"            { printf("%s", yytext); return ELSE; }
"while"           { printf("%s", yytext); return WHILE; }

"extern"          { printf("%s", yytext); return EXTERN; }
"print"           { printf("%s", yytext); return PRINT; }
"return"          { printf("%s", yytext); return RETURN; }
"void"            { printf("%s", yytext); return VOID; }
"int"             { printf("%s", yytext); return INT; }
"read"            { printf("%s", yytext); return READ; }



{letter}+	        {   
                      printf("%s", yytext);
                      return ID;
                  }
                  
{letter}+[0-9]+	  {   
                      printf("%s", yytext);
                      return ID;
                  }


{digit}+      		{   
                      printf("%s", yytext);
                      return NUM;
                  }

0                 {
                      printf("%s", yytext);
                      return NUM; 
                  }

[0-9]+            {
                      printf("%s", yytext);
                      return NUM; 
                  }

[-()<>=+*/;.]     { 
                      printf("%s", yytext); 
                      return yytext[0];;
                  }

"{"               { printf("%s", yytext); return yytext[0];}
"}"               { printf("%s", yytext); return yytext[0]; }
"<="              { printf("%s", yytext); return LE; }
">="              { printf("%s", yytext); return GE; }
"=="              { printf("%s", yytext); return EQ; }
"!="              { printf("%s", yytext); return NE; }


[ \t\n]+          { printf("%s", yytext); }

%%


int yywrap(void){
	return 1;

}



