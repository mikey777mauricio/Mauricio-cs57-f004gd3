/* 
 * Mikey Mauricio     Codegen Module    May 2nd, 2023
 *
 * codegen.h - header file for 'optimizer' module 
 * 
 * the `codegen` module implements register allocation and 
 *   generates machine assembly code given LLVM module 
 *
 */
/**************** Link Section ****************/
#ifndef CODEGEN_H
#define CODEGEN_H
#include <cstddef>
#include <bits/stdc++.h>

#include<vector>
#include<unordered_set> 
#include<utility> 
#include <unordered_map>
using namespace std;
#include <iostream>
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
void compute_liveness(LLVMBasicBlockRef bb, 
        unordered_map<int, LLVMValueRef>& inst_index, unordered_map<LLVMValueRef, pair<int, int>>& live_range);

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
unordered_map<LLVMValueRef, int> register_allocation(LLVMValueRef fn);

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
vector<LLVMValueRef> sort_list(unordered_map<LLVMValueRef, pair<int, int>> live_range); 

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
LLVMValueRef find_spill(unordered_map<LLVMValueRef, int> reg_map, vector<LLVMValueRef> sorted);

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
unordered_map<LLVMBasicBlockRef, const char*> createBBLabels(LLVMValueRef fn);

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
void printDirectives(const char* label); 

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
void printFunctionEnd();

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
unordered_map<LLVMValueRef,int> getOffsetMap(LLVMValueRef fn);

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
void code_gen(LLVMModuleRef m);

#endif