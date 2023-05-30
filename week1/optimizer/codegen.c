/* 
 * Mikey Mauricio     codegen Module    May 2nd, 2023
 *
 * codegen.h - header file for 'codegen' module 
 * 
 * the `codegen` module implements register allocation and 
 *   generates machine assembly code given LLVM module  
 *
 */

/**************** Link Section ****************/
#include "codegen.h"
 
/**************** functions ****************/

/**************** compute_liveness() ****************/
/* see codegen.h for description */ 
void compute_liveness(LLVMBasicBlockRef bb, unordered_map<LLVMValueRef, int> inst_index, unordered_map<LLVMValueRef, pair<int, int>> live_range){
  // iterate through each bb to set index 
  int index = 0; 
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; 
          instruction = LLVMGetNextInstruction(instruction)){
    // check if not an alloca instruction
    if (!LLVMIsAAllocaInst(instruction)){
      inst_index[instruction] = index; 
      index++; 
    
    }
  }
  // iterate through instructions to compute live_range 
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; 
          instruction = LLVMGetNextInstruction(instruction)){
    // ignore alloca
    if (!LLVMIsAAllocaInst(instruction)){
      // set map key, value
      pair<int, int> range(inst_index[instruction], inst_index[instruction]);
      // iterate through operands
      int num_operands = LLVMGetNumOperands(instruction); 
      for (int i = 0; i < num_operands; i++) {
        LLVMValueRef operand = LLVMGetOperand(instruction, i);
        // search for operand 
        if (inst_index.find(operand) != inst_index.end()){
          // set range 
          range.first = std::min(range.first, inst_index[operand]);
          range.second = std::max(range.second, inst_index[operand]);
        
        }
      }
      // set instruction value to range 
      live_range[instruction] = range;
    }
  }
}

/**************** register_allocation() ****************/
/* see codegen.h for description */ 
unordered_map<LLVMValueRef, int> register_allocation(LLVMValueRef fn){
  // initialize reg map 
  unordered_map<LLVMValueRef, int> reg_map; 
  
  // loop through basic blocks 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; 
          bb = LLVMGetNextBasicBlock(bb)){
    // initialize set of physical registers 
    unordered_set<int> registers = {0, 1, 2,}; // ebx, ecx, edx
    // get inst_index and get live range 
    unordered_map<LLVMValueRef, int> inst_index;
    unordered_map<LLVMValueRef, pair<int, int>> live_range;
    compute_liveness(bb, inst_index, live_range);
    // iterate through instructions 
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
            instruction = LLVMGetNextInstruction(instruction)){
      // ignore if its an alloc 
      if (LLVMIsAAllocaInst(instruction)) continue; 
      // get num operands 
      int num_operands = LLVMGetNumOperands(instruction);
      // if instr is an instruction that does not have a result 
      if (!LLVMIsAStoreInst(instruction) && !LLVMIsABranchInst(instruction)){
        // iterate operands 
        for (int i = 0; i < num_operands; i++){
          // get value ref
          LLVMValueRef op = LLVMGetOperand(instruction, i);
          // continue if not in reg map 
          if (reg_map.find(op) == reg_map.end()) continue; 
          // if live range of op ends and has a physical register 
          if (live_range[op].second < inst_index[instruction] && reg_map[op] != -1){
            // add register to set 
            registers.insert(reg_map[op]);

          }
        }
      }
      
      //if is of type add/sub/mul   
      LLVMOpcode op_code = LLVMGetInstructionOpcode(instruction);
      if (op_code == LLVMAdd || op_code == LLVMSub || op_code == LLVMMul){
        // if first operand has a physical register
        LLVMValueRef first_op = LLVMGetOperand(instruction, 0);
        // if it has a physical register 
        if (reg_map.find(first_op) != reg_map.end() && reg_map[first_op] >= 0){
          // if liveness range of first op ends at instr
          if (inst_index[instruction] ==  live_range[first_op].second) {
            // add instruction to R
            reg_map[instruction] = reg_map[first_op];
            // get second operand 
            LLVMValueRef second_op = LLVMGetOperand(instruction,1);
            // if liveness range ends and in reg map 
            if (live_range[second_op].second <= inst_index[instruction] 
                    && reg_map.find(second_op) != reg_map.end()){
              // if has physical register 
              if (reg_map[second_op] != -1) {
                // add to set of registers  
                registers.insert(reg_map[second_op]);

              }
            }
          }

        }
      }
      // if a physical register R is available 
      else if (!registers.empty()){
        for (auto reg : registers){
          reg_map[instruction] = reg; 
          registers.erase(reg);
          break; 
        }
        // iterate through operands
        for (int i = 0; i < num_operands; i++){
          LLVMValueRef op = LLVMGetOperand(instruction, i);
          // if ends and has physical register 
          if (live_range[op].second <= inst_index[instruction] && reg_map.find(op) != reg_map.end()){
            if (reg_map[op] != -1){
              // add to available 
              registers.insert(reg_map[op]);

            }
          }
        }
      }  
      // c) liveness range of the first operand ends at Instr, then
      else if (registers.empty()){
        // get sorted list 
        vector<LLVMValueRef> sorted = sort_list(live_range);
        // call find spill to get V
        LLVMValueRef V = find_spill(reg_map, sorted);
        // if V has more uses than Inst 
        if (live_range[instruction].second > live_range[V].second){
          // add the entry inst -> -1 to reg map to indicate no register
          reg_map[instruction] = -1; 

        } else {
          // get the physical register R assigned to V in reg_map 
          int R = reg_map[V];
          // add the entry instr->R to reg map 
          reg_map[instruction] = R; 
          // update the entry for V from V->R to V-> -1 in reg map 
          reg_map[V] = -1; 

        }
        // iterate through operands 
        for (int i = 0; i < num_operands; i++){
          // get value ref
          LLVMValueRef op = LLVMGetOperand(instruction, i);
          // if not in map continue
          if (reg_map.find(op) == reg_map.end()) continue; 
          // if operand range ends and assigned to physical register 
          if (live_range[op].second <= inst_index[instruction] && reg_map[op] > -1){
            // add to set of available registers 
            registers.insert(reg_map[op]); 
      
          }
        }
      }
    }
  }
  return reg_map; 
}

/**************** sort_list() ****************/
/* see codegen.h for description */ 
vector<LLVMValueRef> sort_list(unordered_map<LLVMValueRef, pair<int, int>> live_range){
  // initalize list 
  vector<LLVMValueRef> sorted; 
  // add each instruction to list 
  for (const auto &key : live_range){
    sorted.push_back(key.first); 

  }
  // insertion sort based on end points 
  for (unsigned int i = 1; i < sorted.size(); i++){
    // set key to instruction 
    LLVMValueRef key = sorted[i];
    // get previous instruction 
    int j = i - 1; 
    // keep pushing j back until end point is less than next instruction 
    while (j >= 0 && live_range[sorted[j]].second < live_range[key].second){
      // move back j 
      sorted[j+1] = sorted[j];
      // decrement j 
      j--; 
    }
    // set key to position after j 
    sorted[j+1] = key;

  }
  // return list 
  return sorted; 

}

/**************** find_spill() ****************/
/* see codegen.h for description */ 
LLVMValueRef find_spill(unordered_map<LLVMValueRef, int> reg_map, vector<LLVMValueRef> sorted){
  // iterate through sorted list 
  for (auto instruction: sorted){
    // if has physical register
    if (reg_map.find(instruction) != reg_map.end() && reg_map[instruction] != -1){
      // return instruction as V
      return instruction; 

    }
  }
  // return null if reaches end
  return NULL; 

}

/**************** createBBLabels() ****************/
/* see codegen.h for description */
unordered_map<LLVMBasicBlockRef, const char*> createBBLabels(LLVMValueRef fn){

  unordered_map<LLVMBasicBlockRef, const char*> labels; 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb;
          bb = LLVMGetNextBasicBlock(bb)){
    // get label 
    const char* label = LLVMGetBasicBlockName(bb);
    // set bb -> label 
    labels[bb] = label; 

  }
  // return 
  return labels; 

}

/**************** printDirectives() ****************/
/* see codegen.h for description */ 
void printDirectives(const char* label){
  fprintf(stdout, ".text\n");
  fprintf(stdout, ".global %s\n", label);
  fprintf(stdout, ".type %s, @function\n", label);
  fprintf(stdout, "%s:\n", label);
  // push ebp onto stack 
  fprintf(stdout, "pushl %%ebp\n");
  // copy esp into ebp 
  fprintf(stdout, "movl %%esp, %%ebp\n");

}

/**************** printFunctionEnd() ****************/
/* see codegen.h for description */ 
void printFunctionEnd(){
  // copy ebp into esp 
  fprintf(stdout, "movl %%ebp, %%esp\n");
  // pop ebp from stack 
  fprintf(stdout, "popl %%ebp\n");
  // return instruction 
  fprintf(stdout, "ret\n");

}

/**************** getOffsetMap() ****************/
/* see codegen.h for description */ 
unordered_map<LLVMValueRef,int> getOffsetMap(LLVMValueRef fn){
  // initialize map 
  unordered_map<LLVMValueRef, int> offset_map;
  // set offset to 0 
  int offset = 0; 
  // iterate through each basic block 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb;
          bb = LLVMGetNextBasicBlock(bb)){
    // iterate through instructions 
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; 
            instruction = LLVMGetNextInstruction(instruction)){
      // increment offset 
      offset += 4; 
      // populate map 
      offset_map[instruction] = -offset; 

    }
  }
  // return map 
  return offset_map; 

}


/**************** code_gen() ****************/
/* see codegen.h for description */ 
void code_gen(LLVMModuleRef m){
  // vector of registers 
  vector<char*> registers = {"ebx", "ecx", "edx"};
  // set stdout 
  // iterate through fuctions 
  for (LLVMValueRef fn = LLVMGetFirstFunction(m); fn; 
          fn = LLVMGetNextFunction(fn)){
    // get label map 
    unordered_map<LLVMBasicBlockRef, const char*> labels = createBBLabels(fn);
    // get reg map 
    unordered_map<LLVMValueRef, int> reg_map = register_allocation(fn); 
    // call printDirectives 
    printDirectives(LLVMGetValueName(fn));
    // get offset map 
    unordered_map<LLVMValueRef,int> offset_map = getOffsetMap(fn);
    // iterate through each basic block 
    for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb;
            bb = LLVMGetNextBasicBlock(bb)){
      // for each instruction 
      for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
              instruction = LLVMGetNextInstruction(instruction)){
        // if is a ret (ret i32 A){
          if (LLVMIsAReturnInst(instruction)){
            // get operand 
            LLVMValueRef A = LLVMGetOperand(instruction, 0);
            // if constant 
            if (LLVMIsConstant(A)){
              // extract value 
              int val = LLVMConstIntGetSExtValue(A);
              // emit movl $A, %eax
              fprintf(stdout, "movl $%d, %%eax\n", val);

            }
            // if A is a temp var and in mem
            else if (offset_map.find(A) != offset_map.end()){
              // get offset 
              int offset = offset_map[A];
              // emit movl k(%ebp), %eax 
              fprintf(stdout, "movl %d(%%ebp), %%eax\n", offset); 

            }

            else if (reg_map.find(A) != reg_map.end()){
              if (reg_map[A] != -1){
                char* reg = registers[reg_map[A]];
                // emit movl %exx, %eax 
                fprintf(stdout, "movl %%%s, %%eax\n", reg);

              }
            }
            // call printFunctionEnd
            printFunctionEnd();

          }
          // if instr is a load 
          else if (LLVMIsALoadInst(instruction)){
            LLVMValueRef first_op = LLVMGetOperand(instruction, 0);
            if (reg_map.find(first_op) != reg_map.end()){
              if (reg_map[first_op] != -1){
                char* reg = registers[reg_map[first_op]];
                LLVMValueRef b = LLVMGetOperand(instruction, 1);
                // get offset of b 
                int c = offset_map[b];
                // emit movl c(%ebp), %exx
                fprintf(stdout, "movl %d(%%ebp), %%%s\n", c, reg);

              }
            }
          }
          // if store 
          else if (LLVMIsAStoreInst(instruction)){
            // get A 
            LLVMValueRef A = LLVMGetOperand(instruction, 0);
            // if param ignore 
            if (LLVMIsAArgument(A)) continue; 
            // B
            LLVMValueRef B = LLVMGetOperand(instruction, 0);
            // if parameter
            if (LLVMIsConstant(A)){
              int val = LLVMConstIntGetSExtValue(A);
              // get offset c of %b 
              int c = offset_map[B];
              // emit movl $A, c(%ebp)
              fprintf(stdout, "movl $%d, %d(%%ebp)\n", val, c);

            }
            // if a is a temp variable 
            else{
              // if it has a assigned register 
              if (reg_map.find(A) != reg_map.end() && reg_map[A] != -1){
                  // get off set of b 
                  int c = offset_map[B];
                  char* reg = registers[reg_map[A]];
                  // emit movl %exx, c(%ebp)
                  fprintf(stdout, "movl %%%s, %d(%%ebp)\n", reg, c);

              } else if (offset_map.find(A) != offset_map.end()) {
                // get offset c1 of a 
                int c1 = offset_map[A];
                // emit movl c1(%ebp), %eax 
                fprintf(stdout, "movl %d(%%ebp), %%eax\n", c1);
                // get offset c2 of b 
                int c2 = offset_map[B];
                // emit movl %eax, c2(%ebp)
                fprintf(stdout, "movl %%eax, %d(%%ebp)\n", c2);
                
              }
            } 
          }
          // if call 
          if (LLVMIsACallInst(instruction)){
            // emit pushl %ebx
            fprintf(stdout, "pushl %%ebx\n"); 
            // emit pushl %ecx
            fprintf(stdout, "pushl %%ecx\n"); 
            // emit pushl %edx
            fprintf(stdout, "pushl %%edx\n"); 
            // get call func 
            LLVMValueRef call = LLVMGetCalledValue(instruction);
            // if has params 
            if (LLVMCountParams(call) > 0){
              LLVMValueRef P =  LLVMGetFirstParam(call);
              // if P is a const 
              if (LLVMIsConstant(P)){
                int val = LLVMConstIntGetSExtValue(P);
                // emit pushl $P
                fprintf(stdout, "pushl $%d\n", val);

              }
              // if temp 
              else {
                // if P has a physical register 
                if (reg_map.find(P) != reg_map.end() && reg_map[P] != -1){
                  // emit pushl %exx 
                  char* reg = registers[reg_map[P]];
                  fprintf(stdout, "pushl %%%s\n", reg);

                }
                // if p is in memory 
                if (offset_map.find(P) != offset_map.end()){
                  // get off setg k of %p 
                  int k = offset_map[P];
                  // emit pushl k(%ebp)
                  fprintf(stdout, "pushl %d(%%ebp)\n", k);

                }
                // emit call func 
                fprintf(stdout, "call %s\n", LLVMGetValueName(call)); 
              }
              // if func has param P 
              if (LLVMCountParams(call) > 0){
                
                // emit addl $4, %esp
                fprintf(stdout, "addl $4, %%esp\n");

              }
              // emit popl %edx
              fprintf(stdout, "popl %%edx\n");
              // emit popl %ecx
              fprintf(stdout, "popl %%ecx\n");
              // emit popl %ebx
              fprintf(stdout, "popl %%ebx\n");

            }
            // if instr is of the form %a = call type @func 
            int num_operands = LLVMGetNumOperands(instruction);
            if (num_operands > 1){
              // get A 
              LLVMValueRef A = LLVMGetOperand(instruction, 0);
              // if A has a physical register 
              if (reg_map.find(A) != reg_map.end() && reg_map[A] != -1){
                // emit movl %eax, %exx 
                char* reg = registers[reg_map[A]];
                fprintf(stdout, "movl %%eax, %%%s\n", reg);
              }
              // if A is in memory
              if (offset_map.find(A) != offset_map.end()){
                // get off set
                int k = offset_map[A];
                // emit movl %eax, k(%ebp) 
                fprintf(stdout, "movl %%eax, %d(%%ebp)\n", k);

              }
            }
          }
          // if branch instruction 
          if (LLVMIsABranchInst(instruction)){
            if(!LLVMIsConditional(instruction)){
              // get label L from bb_labels 
              const char* label = labels[LLVMGetSuccessor(instruction, 0)];
              // emit jmp L 
              fprintf(stdout, "jmp %s\n", label);

            }
            else {
              // get condition 
              LLVMValueRef cond = LLVMGetCondition(instruction);
              // get lebls l1 and l2 
              LLVMBasicBlockRef b1 = LLVMGetSuccessor(instruction, 0);
              LLVMBasicBlockRef b2 = LLVMGetSuccessor(instruction, 1);
              const char* L1 = labels[b1];
              const char* L2 = labels[b2];
              // get the predicate T of comparaisions from A
              LLVMIntPredicate T = LLVMGetICmpPredicate(cond);
              // based on value of T 
              // emit jxx L1 (replace jxx with conditional jump)
              switch (T){
                // less than 
                case LLVMIntSLT: 
                  fprintf(stdout, "jl %s\n", L1);
                  break; 
                // more than 
                case LLVMIntSGT: 
                  fprintf(stdout, "jg %s\n", L1);
                  break;
                // less than equal 
                case LLVMIntSLE: 
                  fprintf(stdout, "jle %s\n", L1);
                  break;
                // more than equal 
                case LLVMIntSGE: 
                  fprintf(stdout, "jge %s\n", L1);
                  break;
                // equal 
                case LLVMIntEQ: 
                  fprintf(stdout, "je %s\n", L1);
                  break;
                // not equal 
                case LLVMIntNE: 
                  fprintf(stdout, "jne %s\n", L1);
                  break;
                default: 
                  break; 

              }
              // emit jmp l2 
              fprintf(stdout, "jmp %s\n", L2);

            }
          }
          // ignore if alloc 
          if (LLVMIsAAllocaInst(instruction)) continue; 
          // if arithmetic 
          else if (LLVMGetInstructionOpcode(instruction) == LLVMAdd || LLVMGetInstructionOpcode(instruction) == LLVMSub
                  || LLVMGetInstructionOpcode(instruction) == LLVMICmp ||LLVMGetInstructionOpcode(instruction) == LLVMMul) {
            char* X;
            // A 
            LLVMValueRef A = LLVMGetOperand(instruction, 0);
            // B
            LLVMValueRef B = LLVMGetOperand(instruction, 1);
            // opcode 
            char* op_directive; 
            LLVMOpcode op = LLVMGetInstructionOpcode(instruction);
            if (op == LLVMAdd) op_directive = "addl";
            if (op == LLVMSub) op_directive = "subl";
            if (op == LLVMICmp) op_directive = "cmpl";
            if (op == LLVMMul) op_directive = "imull";

            // if A has a physical register 
            if (reg_map.find(A) != reg_map.end() && reg_map[A] != -1){
              // X = %exx
              X = registers[reg_map[A]];
              
            } else {
              // x = eax 
              X = "eax";

            }
            // if A is a constant 
            if (LLVMIsConstant(A)){
              // get cal 
              int val = LLVMConstIntGetSExtValue(A);
              // emit movl $A, X
              fprintf(stdout, "movl $%d, %%%s\n", val, X);

            }
            // if temp 
            else {
              // if A has a physical register %eyy assigned
              if (reg_map.find(A) != reg_map.end() && reg_map[A] != -1){
                // emit movl %eyy, X
                char* reg = registers[reg_map[A]];
                fprintf(stdout, "movl %%%s, %s\n", reg, X);

              } 
              if (offset_map.find(A) != offset_map.end()){
                int n = offset_map[A];
                // emit movl n(%ebp), X
                fprintf(stdout, "movl %d(%%ebp), %s\n", n, X);

              }
            }
            // if B is a constant 
            if (LLVMIsConstant(B)){
              //emit addl $B, X
              int val = LLVMConstIntGetSExtValue(B);
              fprintf(stdout, "movl %d(%%ebp), %s\n", val, X);
              
            }
            // if b is temp and has register 
            else if (reg_map.find(B) != reg_map.end() && reg_map[B] != -1){
              // emit addl %ezz, X
              char* b_reg = registers[reg_map[B]];
              fprintf(stdout, "%s %%%s, %s\n", op_directive, b_reg, X);
            }
            else if (reg_map.find(B) == reg_map.end() || reg_map[B] == -1){
              // get off set m of B 
              int m = offset_map[B];
              // emit addl m(%ebp), X
              fprintf(stdout, "%s %d(%%ebp), %s\n", op_directive, m, X);
            }

            if (offset_map.find(A) != offset_map.end()){
              // get offset k of a 
              int k = offset_map[A];
              // emit movl %eax, k(%ebp)
              fprintf(stdout, "movl %%eax, %d(%%ebp)\n", k);
            }
          }
        }
      }
    }
  }






