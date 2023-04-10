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

// left-associatives 
%left GE LE EQ NE '>' '<'

%left '+' '-'
%left '*' '/'

// non-associatives 
%nonassoc UMINUS
%nonassoc IFX
%nonassoc ELSE

// start 
%start program


%%

// program with headers and functions
program : headers functions
        | functions
        ;

// headers
headers : headers header
        | header 
        ;

// header 
header : EXTERN VOID PRINT '(' INT ')' ';'
        | EXTERN INT READ '(' ')' ';'
        ;

// functions 
functions : functions function 
        | function
        ;

// function 
function : INT ID '(' INT ID ')' blocks
        | INT ID '(' ')' blocks
        ;

// blocks 
blocks: blocks block  
      | block
      ;

// block of code
block : '{' statements '}'
    ;

// statements
statements : statements statement 
    | statement 
    ;
    
// statement 
statement : ID '=' expr ';'
    | expr ';'
    | ID '=' READ '(' ')' ';'
    | PRINT expr ';'
    | IF '(' expr ')' statement %prec IFX 
    | IF '(' expr ')' statement ELSE statement
    | WHILE '(' expr ')' statement
    | RETURN '(' expr ')' ';' 
    | '{' statements '}'
    ;
    
// expressions   
expr : term
    | INT ID 
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


    
term : ID 
    | NUM

%%

int main(int argc, char** argv){
  // to read file name as argument
	if (argc == 2){
  	yyin = fopen(argv[1], "r");
		yyparse();
  	fclose(yyin);
	}

  // clean up leaks
	yylex_destroy();
	return 0;
}

void yyerror(char *s){
	fprintf(stdout, "Syntax error %d\n", yylineno);
}


