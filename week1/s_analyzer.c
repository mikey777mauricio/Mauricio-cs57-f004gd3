/* 
 * s_analyzer.c - CS57 's_analyzer' module
 *
 * see s_analyzer.h for more information.
 *
 * Mikey Mauricio,  April 18, 2023
 */

/**************** Link Section ****************/
#include "s_analyzer.h"

/**************** global functions ****************/
void analyze_node(astNode* root);

/**************** local functions ****************/
void analyze_helper(astNode* node, vector<vector<char*>*> *s, vector<char*> *s_table);
void analyze_stmt(astStmt* stmt, vector<vector<char*>*> *s, vector<char*> *s_table);

/**************** functions ****************/

/**************** analyze_stmt() ****************/
/* see s_analyzer.h for description */
void analyze_stmt(astStmt* stmt, vector<vector<char*>*> *s, vector<char*> *s_table){
  // assert function is not NULL
  assert(stmt != NULL);
  // switch 
  switch (stmt->type)
  {
    // block
    case ast_block: {
      printf("block\n");
      // new vector to represent symbol table 
      vector<char*> *new_list;
      new_list = new vector<char*> ();
      // push new symbol table onto vector 
      s->push_back(new_list);
      // list of node
      vector<astNode*> slist = *(stmt->block.stmt_list);
      // iterator to iterate nodes
      vector<astNode*>::iterator it = slist.begin();
      // while not at end 
      while (it != slist.end()){
        // analye new node
        analyze_helper(*it, s, new_list);
        // increment 
        it++;
			
      }
      break;
    
    }
    // call
    case ast_call: {
      printf("call\n");
      // if function has param 
      if (stmt->call.param != NULL){
        // anaylze param 
        analyze_helper(stmt->call.param, s, s_table);

      }
      break;

    }
    // while
    case ast_while:{
      printf("while\n");
      // analyze condition 
      analyze_helper(stmt->whilen.cond, s, s_table);
      // analyze body 
      analyze_helper(stmt->whilen.body, s, s_table);
      break;

    }
    // if 
    case ast_if: {
      printf("if\n");
      // analyze condition 
      analyze_helper(stmt->ifn.cond, s, s_table);
      // analyze body 
      analyze_helper(stmt->ifn.if_body, s, s_table);
      // if else stmt
      if(stmt->ifn.else_body != NULL){
        // analyze else stmt
        analyze_helper(stmt->ifn.else_body, s, s_table);

      }
      break;
      
    }
    // assignment
    case ast_asgn: {
      printf("asgn\n");
      // analyze lhs 
      analyze_helper(stmt->asgn.lhs, s, s_table);
      // analyze rhs 
      analyze_helper(stmt->asgn.rhs, s, s_table); 
      break;

    }
    // declaration
    case ast_decl: {
      printf("decl\n");
      // extract ID
      char* new_id = stmt->decl.name; 
      printf("added %s\n", new_id);
      // push ID onto symbol table 
      s_table->push_back(new_id);
      break;

    }
    // return
    case ast_ret: {
      printf("ret\n");
      // analyze expr 
      analyze_helper(stmt->ret.expr, s, s_table);
      break;

    }
  }
 }

/**************** analyze_helper() ****************/
/* see s_analyzer.h for description */
void analyze_helper(astNode* node, vector<vector<char*>*> *s, vector<char*> *s_table){
  // assert node is not NULL
  assert(node != NULL);
  // switch statement
  switch (node->type)
  {
    // prog 
    case ast_prog: {
      // analyze func
      analyze_helper(node->prog.func, s, s_table);
      break;

    }
    // extern
    case ast_extern: {
      printf("extern\n");
      // extract ID
      char* ext_id = node->ext.name; 
      // push onto symbol table 
      s_table->push_back(ext_id);
      break;

    }
    // func
    case ast_func:{
      printf("func\n");
      // if func has param 
      if (node->func.param != NULL) {
        // extract var node 
        astNode* var = node->func.param;
        // add ID onto symbol table 
        s_table->push_back(var->var.name);
        printf("added parameter\n");
      }
      // analyze func body 
      analyze_helper(node->func.body, s, s_table);
      break;

    }
    // var 
    case ast_var: {
      printf("var\n");
      // found boolean 
      bool found = false; 
      // iterate through vector of symbol tables 
      for (int i = 0; i < (int)s->size(); i++){
        // set list to current table 
        vector<char*> *list = (*s)[i];
        // iterate through table 
        for(int j = 0; j < (int)list->size(); j++){
          printf("looking for: %s, var: %s\n", node->var.name, (*list)[j]);
          // if found 
          if (strcmp(node->var.name, (*list)[j]) == 0){
            printf("found %s\n", node->var.name);
            // set found to true 
            found = true; 
            break; 

          }
        }
      }
      // if not in scope or not declared, print syntax error 
      if (!found) printf("syntax error, out of scope\n");
      break;

    }
    // stmt
    case ast_stmt: {
      // extract stmt 
      astStmt stmt = node->stmt; 
      // analyze stmt 
      analyze_stmt(&stmt, s, s_table);
      break;

    }
    // rexpr
    case ast_rexpr: {
      printf("rexp\n");
      // analyze lhs 
      analyze_helper(node->rexpr.lhs, s, s_table);
      // analyze rhs 
      analyze_helper(node->rexpr.rhs, s, s_table);
      break;

    }
    // bexpr
    case ast_bexpr: {
      printf("bexp\n");
      // analyze lhs 
      analyze_helper(node->bexpr.lhs, s, s_table);
      // analyze rhs 
      analyze_helper(node->bexpr.rhs, s, s_table);
      break;

    }
    // uexpr
    case ast_uexpr: {
      printf("uexp\n");
      // analyze expr 
      analyze_helper(node->uexpr.expr, s, s_table);
      break; 

    }

    default:
      break;
    }
}

/**************** analyze_node() ****************/
/* see s_analyzer.h for description */
void analyze_node(astNode* root){
  // initialize vector of vectors
  vector<vector<char*>*> *s;
  // set s to new vector
  s = new vector<vector<char*>*> (); 
  // initialize vector of char*
  vector<char*> *slist;
  // set slist to vecot
  slist = new vector<char*> (); 
  // push slist onto s 
  s->push_back(slist);
  // analyze root
  analyze_helper(root, s, slist);

}
