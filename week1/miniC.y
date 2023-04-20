%{
#include "ast.h"
#include "s_analyzer.h"
#include <stdio.h>
#include <cstddef>
#include <vector>
#include <stack>
#include <cassert>
using namespace std;
void yyerror(const char *);
extern int yylex(void);
extern int yylex_destroy();
extern FILE *yyin;
extern int yylineno;
astNode* root; 
extern void analyze_node(astNode*);
extern void analyze_helper(astNode* node, stack<vector<char*>*> *s, vector<char*> *s_table);
%}


%union {
  int iVal; 
  char* sName; 
  astNode* nptr;
  vector<astNode*> *stmt_list;

};


// defining tokens 
%token <iVal> NUM
%token <sName> ID  

%token WHILE IF EXTERN  VOID INT ELSE READ RETURN PRINT

// left-associatives 
%left GE LE EQ NE '>' '<'

%left '+' '-'
%left '*' '/'

// non-associatives 
%nonassoc UMINUS
%nonassoc IFX
%nonassoc ELSE

%type <nptr> program extern function_def block
%type <nptr> statement expr term declaration
%type <stmt_list> declarations statements

// start 
%start program

%debug

%%

// program with headers and functions
program : extern extern function_def { 
                                        $$ = createProg($1, $2, $3); 
                                        root = $$;
                                        analyze_node($$);
                                      
                                      }
        ;

// extern 
extern : EXTERN VOID PRINT '(' INT ')' ';' { $$ = createExtern("print");  }
        | EXTERN INT READ '(' ')' ';' { $$ = createExtern("read");  }
        ;

function_def : INT ID '(' INT ID ')'  statement { astNode* var = createVar($5); $$ = createFunc($2, var, $7); free($2);  free($5);}
        | INT ID '(' ')' statement {  $$ = createFunc($2, NULL, $5); free($2); }
        ;

block : '{' declarations statements '}' { 

                                          for(int i = 0; i < (*$3).size(); i++){
                                              $2->push_back((*$3)[i]);

                                          }              
                                          delete $3;                            
                                          $$ = createBlock($2);

                                        }
      ;

// statements
statements : statements statement {
                                    $1->push_back($2);
                                    $$ = $1;                             
                                  } 
    | statement                   {                                   
	                                  $$ = new vector<astNode*> ();                           
                                    $$->push_back($1);
                                  }
    ;
    
// statement 
statement : ID '=' expr ';' {   
                                $$ = createAsgn(createVar($1), $3); 
                                free($1);
                              }
    | expr ';' { $$ = $1; }
    | PRINT expr ';'            { 
                                    $$ = createCall("print", $2);  
                                                                 
                                }
    | IF '(' expr ')' statement %prec IFX { 
                                            $$ = createIf($3, $5);                                          
                                          }
    | IF '(' expr ')' statement ELSE statement { 
                                                $$ = createIf($3, $5, $7);                                                                                       
                                                }
    | WHILE '(' expr ')' block { 
                                    $$ = createWhile($3, $5);                               
                                    } 
    | RETURN '(' expr ')' ';' { $$ = createRet($3); }
    | block { $$ = $1;}
    ;
    
// expressions   
expr : term 
    |'-' term %prec UMINUS { $$ = createUExpr($2, uminus); }
    | expr '+' expr  { $$ = createBExpr($1, $3, add); }
    | expr '-' expr  { $$ = createBExpr($1, $3, sub); }
    | expr '*' expr  { $$ = createBExpr($1, $3, mul); }
    | expr '/' expr  { $$ = createBExpr($1, $3, divide); }
    | expr '<' expr %prec GE { $$ = createRExpr($1, $3, lt); }
    | expr '>' expr %prec LE { $$ = createRExpr($1, $3, gt); }
    | expr EQ expr { $$ = createRExpr($1, $3, eq); }
    | expr NE expr { $$ = createRExpr($1, $3, neq); }
    | expr LE expr { $$ = createRExpr($1, $3, le); }
    | expr GE expr { $$ = createRExpr($1, $3, ge); }
    | '(' expr ')' { $$ = $2; }
    | READ '(' ')' {$$ = createCall("read", NULL); }
    ;

// took out declaration and declarations
declarations : declarations declaration { 
                                          $1->push_back($2);         
                                          $$ = $1;    
                                                                
                                        }
    | { $$ = new vector<astNode*> (); }

declaration : INT ID ';' { $$ = createDecl($2); free($2);}
    
term : ID { $$ = createVar($1); free($1);}
    | NUM { $$ = createCnst($1); }

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

void yyerror(const char *s){
	fprintf(stdout, "Syntax error %d\n", yylineno);
}


