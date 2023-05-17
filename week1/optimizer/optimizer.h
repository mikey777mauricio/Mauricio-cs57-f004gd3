/* 
 * Mikey Mauricio     Optimizer Module    May 2nd, 2023
 *
 * optimizer.h - header file for 'optimizer' module 
 * 
 * the `optimizer` module implements common sub-expression elimination, 
 *    dead code elimination, and constant folding. It returns and 
 *    optimized LLVM module 
 *
 */

/**************** Link Section ****************/
#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H
#include <cstddef>
#include <bits/stdc++.h>

#include<vector>
#include<unordered_set> 
using namespace std;
#include <iostream>

#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>





/**************** functions ****************/

/**************** common_sub_expr() ****************/
/* common_sub_expr(): Goes through instructions in each basic block
 *  and identifies pairs of instructions that have same opcode and operands. 
 *  With those pairings will modify all uses of latest instruction to point
 *  to prior identical instruction. 
 * 
 * Caller provides: LLVMBasicBlockRef bb, basic block 
 * 
 * We return:
 *  true: if modified instructions
 *  false: if no optimization occured
 * We guarantee:
 *   Instructions will not be modified if there is a store 
 *    instruction modifying any of their operands before the
 *    identical instruction is called 
 * Caller is responsible for:
 *   nothing
 */
bool common_sub_expr(LLVMBasicBlockRef bb);

/**************** dead_code_elimination() ****************/
/* dead_code_elimination(): Goes over all instructions in 
 *   given function and removes instructions that do not 
 *   have any use. 
 * 
 * Caller provides: LLVMValueRef function, current function 
 * 
 * We return:
 *   true: if removed instructions
 *   false: if no optimization occured
 * We guarantee:
 *   Instructions having indirect uses or side effects are 
 *      not deleted
 * Caller is responsible for:
 *   nothing
 */
bool dead_code_elimination(LLVMBasicBlockRef block);

/**************** const_folding() ****************/
/* const_folding(): Goes over all instructions in 
 *   given function and finds instructions where opcode 
 *   corresponds to arithmetic operations (+,-,*) and all 
 *   operands are constant. It replaces these instructions to 
 *   point to a constant instruction 
 * 
 * Caller provides: LLVMValueRef function, current function 
 * 
 * We return:
 *   true: if replaced instructions
 *   false: if no optimization occured
 * We guarantee:
 *   Only add, sub, and mult instructions are modified 
 * Caller is responsible for:
 *   nothing
 */
bool const_folding(LLVMBasicBlockRef block);

std::unordered_set<LLVMValueRef> compute_gen(LLVMBasicBlockRef block);
std::unordered_set<LLVMValueRef> compute_set_s(LLVMValueRef fn);
std::unordered_set<LLVMValueRef> compute_kill(LLVMBasicBlockRef block, unordered_set<LLVMValueRef> s);
std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> compute_in_out(LLVMValueRef fn);
unordered_set<LLVMValueRef> find_loads(LLVMValueRef instruction, unordered_set<LLVMValueRef> r);

bool constant_prop(LLVMValueRef fn, unordered_set<LLVMValueRef> s);
bool check_loadInstructions(LLVMValueRef fn, unordered_map<LLVMBasicBlockRef, unordered_set<LLVMValueRef>> in);
/**************** optimize() ****************/
/* optimize(): Optimizes the given LLVMModuleRef by calling 
 *   each optimization function within separate loops.  
 * 
 * Caller provides: LLVMModuleRef m, module to optimize
 * 
 * We return:
 *   optimized LLVMModuleRef
 * We guarantee:
 *   functionality of module will not change
 * Caller is responsible for:
 *   nothing
 */
LLVMModuleRef optimize_mod(LLVMModuleRef m);

#endif