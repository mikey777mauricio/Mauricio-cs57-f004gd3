%{
#include <stdio.h>
void yyerror(char *);
extern int yylex();
extern int yylex_destroy();
extern FILE *yyin;
extern int yylineno;
extern char* yytext;
%}


// defining tokens 
%token NUM
%token  ID
%token WHILE IF PRINT EXTERN RETURN VOID INT READ ELSE
%token LE GE EQ NE

%left GE LE EQ NE '>' '<'

%left '='

%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%nonassoc IFX
%nonassoc ELSE


%start program


%%

program : headers function
  | function
  ;
        

headers : headers header
        | header 
        ;

header : EXTERN VOID PRINT '(' INT ')' ';'
    | EXTERN INT READ '(' ')' ';'
    ;



function : INT ID '(' INT ID ')' blocks
        | INT ID '(' ')' blocks
        ;

blocks: blocks block  
      | block
      ;


block : '{' declarations statements '}'
    | '{' statements '}'
    ;


statements : statements statement 
    | statement 
    ;
    
    

statement : ID '=' expr ';'
    | ID '=' READ '(' ')' ';'
    | expr ';'
    | PRINT expr ';'
    | IF '(' expr ')' statement %prec IFX 
    | IF '(' expr ')' statement ELSE statement
    | WHILE '(' expr ')' block
    | RETURN expr ';' 
    | PRINT expr
    | '{' statements '}'
    ;
    
    
expr : ID
    | NUM
    | expr '+' expr 
    | expr '*' expr 
    | expr '/' expr 
    | expr '<' expr %prec GE
    | expr '>' expr %prec LE
    | expr EQ expr 
    | expr NE expr 
    | expr LE expr 
    | expr GE expr 
    | '(' expr ')'
    ;


declarations : declarations declaration
    | declaration
    ;
    
declaration : INT ID ';'
    ;
    

%%

int main(int argc, char** argv){
	if (argc == 2){
  	yyin = fopen(argv[1], "r");
		yyparse();
  	fclose(yyin);
	}
	yylex_destroy();
	return 0;
}

void yyerror(char *s){
	fprintf(stdout, "Syntax error %d\n", yylineno);
}


