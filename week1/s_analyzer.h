/* 
 * Mikey Mauricio     Semantic Analyzer Module    April 18, 2023
 *
 * s_analyzer.h - header file for 's_analyzer' module 
 * 
 * the `s_analyzer` module checks whether the parse tree follows the
 *  rules of the language. The module implements symbol table using a 
 *  vector of char* that represents a list of valid identifiers in a scope
 *  to check whether identifiers are declared before use or not. In addition,
 *  the module also implements a vector of vector<char*> to store and keep track
 *  of the current scope. 
 *
 */

/**************** Link Section ****************/
#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <stdio.h>
#include <stdlib.h>
#include <cstddef>
#include <vector>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "ast.h"
using namespace std;

/**************** functions ****************/

/**************** analyze_node() ****************/
/* index_new(): Initializes a vector of char* and a new
 *  symbol table which is vector of vector<char*>. Adds the 
 *  symbol table to the vector of vectors and calls analyze helper. 
 * 
 * Caller provides: astNode* root, the root of the AST tree
 * 
 * We return:
 *   1: if syntax error
 *   2: no syntax errors
 * We guarantee:
 *   if no errors, the helper functions will parse the entire 
 *   AST tree. 
 * Caller is responsible for:
 *   nothing
 */
int analyze_node(astNode* root);

/**************** analyze_helper() ****************/
/* index_new(): Asserts the input node is non-NULL. Then 
 *  uses a switch statement to analyze the node 
 *  depending on the node type. 
 * 
 * Caller provides: astNode* node, the current node of the AST tree
 *                  s, the vector of symbol tables
 *                  s_table, the current symbol table
 * 
 * We return:
 *   nothing
 * We guarantee:
 *   The node will be analyzed depending on the node-type.
 * Caller is responsible for:
 *   nothing
 */
void analyze_helper(astNode* node, vector<vector<char*>*> *s, vector<char*> *s_table);

/**************** analyze_stmt() ****************/
/* index_new(): Asserts the input stmt is non-NULL. Then 
 *  uses a switch statement to analyze the stmt 
 *  depending on the stmt type. 
 * 
 * Caller provides: astStmt* stmt, the current stmt of the node analyzing
 *                  s, the vector of symbol tables
 *                  s_table, the current symbol table
 * 
 * We return:
 *   nothing
 * We guarantee:
 *   The stmt will be analyzed depending on the node-type.
 *   Prints syntax error if a identifier is used before declaration
 * Caller is responsible for:
 *   nothing
 */
void analyze_stmt(astStmt* stmt, vector<vector<char*>*> *s, vector<char*> *s_table);

#endif