/* 
 * Mikey Mauricio     Codegen Module    May 2nd, 2023
 *
 * codegen.h - header file for 'optimizer' module 
 * 
 * the `codegen` module implements register allocation and 
 *   generates machine assembly code given LLVMModuleRef to build 
 *   backend of compiler  
 *
 */
/**************** Link Section ****************/
#ifndef CODEGEN_H
#define CODEGEN_H
#include <cstddef>
#include <bits/stdc++.h>
#include <iostream>     // std::cout
#include <algorithm>
#include <vector>
#include <unordered_set> 
#include <utility> 
#include <unordered_map>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>

/**************** functions ****************/

/**************** compute_liveness() ****************/
/* compute_liveness(): Goes through instructions in each basic block
 *  and fills liveness and instruction index maps 
 * 
 * Caller provides: basic block, instruction index map, live range map 
 * 
 * We return:
 *  nothing
 * 
 * Caller is responsible for:
 *   nothing
 */
void compute_liveness(LLVMBasicBlockRef bb, 
        unordered_map<int, LLVMValueRef>& inst_index, unordered_map<LLVMValueRef, pair<int, int>>& live_range);

/**************** register_allocation() ****************/
/* register_allocation(): Goes through instructions in each basic block
 *  and assigns registers and fills register map 
 * 
 * Caller provides: LLVMModuleRef
 * 
 * We return:
 *  register map 
 * 
 * Caller is responsible for:
 *   nothing
 */
unordered_map<LLVMValueRef, int> register_allocation(LLVMModuleRef m);

/**************** find_spill() ****************/
/* find_spill(): Goes through instructions fills spill 
 * 
 * Caller provides: register map and sorted list
 * 
 * We return:
 *  A LLVMValueRef, V, that has a physical register assigned to it
 * 
 * Caller is responsible for:
 *   nothing
 */
LLVMValueRef find_spill(unordered_map<LLVMValueRef, int> &reg_map, vector<pair<LLVMValueRef, pair<int, int>>> &sorted_list);

/**************** create_bb_labels() ****************/
/* create_bb_labels(): This function populates a map where key is a 
 *  LLVMBasicBlockRef and associated value is a char * that you can 
 *  use as label when generating code.
 * 
 * Caller provides: LLVMValueRef fn
 * 
 * We return:
 *  map of bb and labels 
 * 
 * Caller is responsible for:
 *   nothing
 */
unordered_map<LLVMBasicBlockRef, char*> create_bb_labels(LLVMValueRef fn);

/**************** print_directives() ****************/
/* print_directives(): This function emits the required 
 *  directives for your function and also assembly instructions 
 *  to push callers %ebp and update the %ebp to current value of %esp.
 * 
 * Caller provides: FILE* fp, int mem, bool ebx_used
 * 
 * We return:
 *  nothing
 * 
 * Caller is responsible for:
 *   nothing
 */
void print_directives(FILE* fp, int local_mem, bool ebx_used); 

/**************** print_function_end() ****************/
/* print_function_end(): This function emits the assembly instructions 
 *   to restore the value of %esp and %ebp, and the ret instruction. 
 * 
 * Caller provides: FILE* fp, bool ebx_used 
 * 
 * We return:
 *  Nothing
 * 
 * Caller is responsible for:
 *  Nothing
 */
void print_function_end(FILE* fp, bool ebx_used);

/**************** get_offset_map() ****************/
/* get_offset_map(): This function will populate the global map 
 *  offset_map. This map associates each value(instruction) to the 
 *  memory offset of that value from %ebp. The keys in this map are 
 *  LLVMValueRef and values are integers. This function will also 
 *  initialize an integer variable localMem that indicates the number 
 *  of bytes required to store the local values.
 * 
 * Caller provides: LLVMValueRef fn, unordered_map<LLVMValueRef, int> &offset_map,
 *  unordered_map<LLVMValueRef, int> &reg_map
 * 
 * We return:
 *  int to represent local mem address
 * 
 * Caller is responsible for:
 *  nothing
 */
int get_offset_map(LLVMValueRef func, unordered_map<LLVMValueRef, int> &offset_map, unordered_map<LLVMValueRef, int> &reg_map);

/**************** codegen() ****************/
/* codegen(): Goes through instructions in each basic block
 *  and generates machine assembly code from LLVMIR
 * 
 * Caller provides: LLVMModuleRef m, FILE*fp
 * 
 * We return:
 *  nothing
 * 
 * Caller is responsible for:
 *  closing file pointer
 */
void codegen(LLVMModuleRef m, FILE*fp);

/**************** check_instruction() ****************/
/* check_instruction(): Checks each instruction and emits 
 *  assembly code 
 * 
 * Caller provides:LLVMValueRef instr, unordered_map<LLVMBasicBlockRef, 
 *  char*> bb_labels, unordered_map<LLVMValueRef, int> offset_map, 
 *  unordered_map<LLVMValueRef, int> reg_map, bool ebx_used, FILE*fp
 * 
 * We return:
 *  nothing
 * 
 * Caller is responsible for:
 *  Closing file pointer
 */
void check_instruction(LLVMValueRef instr, unordered_map<LLVMBasicBlockRef, char*> bb_labels, unordered_map<LLVMValueRef, int> offset_map, unordered_map<LLVMValueRef, int> reg_map, bool ebx_used, FILE*fp); 

/**************** get_cond_jump() ****************/
/* get_cond_jump(): Given pred, returns directive for cond jump 
 *  
 * Caller provides: LLVMIntPredicate pred
 * 
 * We return:
 *  nothing
 * 
 * Caller is responsible for:
 *  closing file pointer
 */
char *get_cond_jump(LLVMIntPredicate pred);

/**************** get_reg() ****************/
/* get_reg(): Maps int to corresponding register 
 * 
 * Caller provides: int reg_num
 * 
 * We return:
 *  char* register name 
 * 
 * Caller is responsible for:
 *  nothing
 */
char* get_reg(int reg_num);

/**************** get_arith_op() ****************/
/* get_arith_op(): Returns directive for corresponding 
 *  opcode 
 * 
 * Caller provides: LLVMOpcode opcode 
 * 
 * We return:
 *  char* arithmetic 
 * 
 * Caller is responsible for:
 *  nothing
 */
char* get_arith_op(LLVMOpcode opcode);
#endif