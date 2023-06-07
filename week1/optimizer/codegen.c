/* 
 * Mikey Mauricio     codegen Module    May 2nd, 2023
 *
 * codegen.h - header file for 'codegen' module 
 * 
 * the `codegen` module implements register allocation and 
 *   generates machine assembly code given LLVMModuleRef to build 
 *   backend of compiler  
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
    if (LLVMIsAAllocaInst(instruction) || !LLVMGetFirstUse(instruction)) continue;
      // set map key, value
      pair<int, int> range(inst_index[instruction], inst_index[instruction]);
      // max
      int second_r = 0; 
      for (LLVMUseRef use = LLVMGetFirstUse(instruction); use; use = LLVMGetNextUse(use)){
        second_r = max(second_r, inst_index[LLVMGetUser(use)]);
      }
      // set second instruction value to max 
      live_range[instruction].second = second_r;
    
  }
}

/**************** register_allocation() ****************/
/* see codegen.h for description */ 
unordered_map<LLVMValueRef, int> register_allocation(LLVMModuleRef m){
  // initialize reg map 
  unordered_map<LLVMValueRef, int> reg_map; 
  // loop through instruction
  for (LLVMValueRef fn = LLVMGetFirstFunction(m); fn; fn = LLVMGetNextFunction(fn)) {
    // loop through basic blocks 
    for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; 
            bb = LLVMGetNextBasicBlock(bb)){
      // initialize set of physical registers 
      unordered_set<int> registers = {0, 1, 2,}; // ebx, ecx, edx
      // get inst_index and get live range 
      unordered_map<LLVMValueRef, int> inst_index;
      unordered_map<LLVMValueRef, pair<int, int>> live_range;
      compute_liveness(bb, inst_index, live_range);
      // get sorted list 
      vector<pair<LLVMValueRef, pair<int, int>>> sorted_list;
      transform(live_range.begin(), live_range.end(), back_inserter(sorted_list), 
            [](auto &myPair) -> pair<LLVMValueRef, pair<int, int>>{return {myPair.first, myPair.second};});
      sort(sorted_list.begin(), sorted_list.end(), [&](auto a, auto b){ return a.second.second > b.second.second; });
      // iterate through instructions 
      for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
              instruction = LLVMGetNextInstruction(instruction)){
        // ignore if its an alloc 
        if (LLVMIsAAllocaInst(instruction)) continue; 
        // get num operands 
        int num_operands = LLVMGetNumOperands(instruction);
        // if instruction is an instruction that does not have a result 
        if (!LLVMGetFirstUse(instruction)){
          // iterate operands 
          for (int i = 0; i < num_operands; i++){
            // get value ref
            LLVMValueRef op = LLVMGetOperand(instruction, i);
            // continue if not in reg map 
            if (reg_map[op] == -1) continue; 
            // if live range of op ends and has a physical register 
            if (live_range[op].second == inst_index[instruction] && reg_map[op] != -1){
              // add register to set 
              registers.insert(reg_map[op]);

            }
          }
          continue; 
        }
        // if arithmetic 
        if (LLVMIsABinaryOperator(instruction)){
          // if first operand has a physical register
          LLVMValueRef first_op = LLVMGetOperand(instruction, 0);
          // if it has a physical register 
          if (reg_map[first_op] != -1 && live_range[first_op].second == inst_index[instruction]){
              // add instruction to R
              reg_map[instruction] = reg_map[first_op];
              // get second operand 
              LLVMValueRef second_op = LLVMGetOperand(instruction,1);
              // if liveness range ends and in reg map 
              if (live_range[second_op].second == inst_index[instruction] 
                      && reg_map[second_op] != -1){
                
                  registers.insert(reg_map[second_op]);

                
              }
            

          }
        }
        // if a physical register R is available 
        else if (!registers.empty()){
          reg_map[instruction] = *registers.begin(); 
          registers.erase(registers.begin());
          // iterate through operands
          for (int i = 0; i < num_operands; i++){
            LLVMValueRef op = LLVMGetOperand(instruction, i);
            // if ends and has physical register 
            if (live_range[op].second == inst_index[instruction] && reg_map[op] != -1){
              
                registers.insert(reg_map[op]);

              
            }
          }
        }  
        // c) liveness range of the first operand ends at instruction, then
        else if (registers.empty()){
          // call find spill to get V
          LLVMValueRef V = find_spill(reg_map, sorted_list);
          // check 
          if (V == NULL) continue; 
          // if V has more uses than Inst 
          if (live_range[instruction].second > live_range[V].second){
            // add the entry inst -> -1 to reg map to indicate no register
            reg_map[instruction] = -1; 

          } else {
            // get the physical register R assigned to V in reg_map 
            int R = reg_map[V];
            // add the entry instruction->R to reg map 
            reg_map[instruction] = R; 
            // update the entry for V from V->R to V-> -1 in reg map 
            reg_map[V] = -1; 

          }
          // iterate through operands 
          for (int i = 0; i < num_operands; i++){
            // get value ref
            LLVMValueRef op = LLVMGetOperand(instruction, i);

            // if operand range ends and assigned to physical register 
            if (live_range[op].second == inst_index[instruction] && reg_map[op] != -1){
              // add to set of available registers 
              registers.insert(reg_map[op]); 
        
            }
          }
        }
      }
    }
  }
  printf("finished reg\n");
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
LLVMValueRef find_spill(unordered_map<LLVMValueRef, int> &reg_map, vector<pair<LLVMValueRef, pair<int, int>>> &sorted_list){
  // iterate through sorted list 
  for (auto instruction: sorted_list){
    // if has physical register
    if (reg_map.find(instruction.first) != reg_map.end() && reg_map[instruction.first] != -1){
      // return instruction as V
      return instruction.first; 

    }
  }
  // return null if reaches end
  return NULL; 

}

void check_instruction(LLVMValueRef instruction, unordered_map<LLVMBasicBlockRef, 
        char*> bb_labels, unordered_map<LLVMValueRef, int> offset_map, unordered_map<LLVMValueRef, 
        int> reg_map, bool ebx_used, FILE* fp) {
  // if return instruction 
  if (LLVMIsAReturnInst(instruction)) {
      // get operand A
      LLVMValueRef A = LLVMGetOperand(instruction, 0);
      // if A is a constant 
      if (LLVMIsAConstant(A)) {
        char *s = LLVMPrintValueToString(A);
        char *t = s + 4;
        // emit movl $A, %eax
        fprintf(fp, "\tmovl\t$%s, %%eax\n", t);
        LLVMDisposeMessage(s);
        
        // if temp and in mem 
      } else if (reg_map[A] == -1) {
        // fet off set 
        int k = offset_map[A] * -1;
        // emit movl k(%ebp), %eax 
        fprintf(fp,"\tmovl\t%d(%%ebp), %%eax\n", k);
        
        // else
      } else {
        // if temp and physical reg
          char *r = get_reg(reg_map[A]);
          // emit movl %exx, %eax
          fprintf(fp, "\tmovl\t%%%s, %%eax\n", r);
          free(r);

      }
    // call printFunctionEnd
    print_function_end(fp,ebx_used);

  }
  // if load 
  else if (LLVMIsALoadInst(instruction)) {
    LLVMValueRef A = LLVMGetOperand(instruction, 0);
    // if A has an assigned physical reg 
    if (reg_map[A] == -1) return;
    // get offset c for b
    int C = offset_map[LLVMGetOperand(instruction, 1)] * -1;
    char *r = get_reg(reg_map[instruction]);
    // emit movl c(%ebp),%exx
    fprintf(fp, "\tmovl\t%d(%%ebp),%%%s\n", C, r);
    free(r);

  }
  // if store 
  else if (LLVMIsAStoreInst(instruction)) {
    // get A
    LLVMValueRef A = LLVMGetOperand(instruction, 0);
    // get b
    LLVMValueRef b = LLVMGetOperand(instruction, 1);
    // if A is parameter, ignore
    if (LLVMGetFirstParam(LLVMGetBasicBlockParent(LLVMGetInstructionParent(instruction))) == A) {
        return;
    
    // if constant 
    } else if (LLVMIsAConstant(A)) {
      char *s = LLVMPrintValueToString(A);
      char *t = s + 4;
      // get offset C of b
      int c = offset_map[b] * -1;
      // emit movl $A, c(%ebp) 
      fprintf(fp,"\tmovl\t$%s, %d(%%ebp)\n", t, c);
      LLVMDisposeMessage(s);

      // else if temp
    } else {
      // if physical reg 
      if (reg_map[A] != -1) {
        // get offset c of b 
        int c = offset_map[b] * -1;
        char *r = get_reg(reg_map[A]);
        // emit movl %exx, c(%ebp)
        fprintf(fp,"\tmovl\t%%%s, %d(%%ebp)\n", r, c);
        free(r);
        // else 
      } else {
        // get offset c1 snd c2 
        int c1 = offset_map[A] * -1;
        int c2 = offset_map[b] * -1;
        // emit movl c1(%ebp), %eax and movl %eax, c2(%ebp)
        fprintf(fp, "\tmovl\t%d(%%ebp), %%eax\n", c1);
        fprintf(fp, "\tmovl\t%%eax, %d(%%ebp)\n", c2);

      }
    }
  }
  // if call
  else if (LLVMIsACallInst(instruction)) {
    /**
     emit pushl %ebx
      emit pushl %ecx
      emit pushl %edx 
      **/ 
    fprintf(fp, "\tpushl\t%%ebx\n");
    fprintf(fp, "\tpushl\t%%ecx\n");
    fprintf(fp, "\tpushl\t%%edx\n");
    
    LLVMValueRef func = LLVMGetCalledValue(instruction);
    int num_params = LLVMCountParams(func);
    // if func has param P
    if (num_params > 0) {
      LLVMValueRef P = LLVMGetParam(func, 0);
      // if P is a constant 
      if (LLVMIsAConstant(P)) {
          char *s = LLVMPrintValueToString(P);
          char *t = s + 4;
          // emit pushl $P
          fprintf(fp,"\tpushl\t$%s\n", t);
          LLVMDisposeMessage(s);
      // if P is temp 
      } else {
        // if P has physical 
        if (reg_map[P] != -1) {
          char *r = get_reg(reg_map[P]);
          // emit pushl %exx
          fprintf(fp,"\tpushl\t%%%s\n", r);
          free(r);

        }
        // if p in memory 
        else {
          // get off set K 
          int k = offset_map[P] * -1;
          // emit pushl k(%ebp)
          fprintf(fp,"\tpushl\t%d(%%ebp)\n", k);

        }
      }
    }
    // emit call func 
    fprintf(fp,"\tcall\t%s\n", LLVMGetValueName(func));
    // if func has P 
    if (num_params > 0) printf("\taddl\t$4, %%esp\n");
    /**
     emit popl %edx
      emit popl %ecx
      emit popl %ebx
    */

    fprintf(fp,"\tpopl\t%%edx\n");
    fprintf(fp,"\tpopl\t%%ecx\n");
    fprintf(fp,"\tpopl\t%%ebx\n");
    // if in form a = call type 
    if (LLVMGetFirstUse(instruction)) {
      // if a has phys
      if (reg_map[instruction] != -1) {
          char *r = get_reg(reg_map[instruction]);
          // emit movl %eax, %exx
          fprintf(fp,"\tmovl\t%%eax, %%%s\n", r);
          free(r);
      // if a in mem 
      } else {
        // get offset 
        int k = offset_map[instruction] * -1; 
        // emit movl %eax, k(%ebp)
        fprintf(fp,"\tmovl\t%%eax, %d(%%ebp)\n", k);
      }
    }
  }
  // if branch 
  else if (LLVMIsABranchInst(instruction)) {
    int num_operands = LLVMGetNumOperands(instruction);
    // if unconditional 
    if (num_operands == 1) {
      // get label
      char *L = bb_labels[LLVMValueAsBasicBlock(LLVMGetOperand(instruction, 0))];
      // emit jmp L
      fprintf(fp,"\tjmp\t%s\n", L);
    // else if conditional 
    } else {
      // get label 
      char *L1 = bb_labels[LLVMValueAsBasicBlock(LLVMGetOperand(instruction, 2))];
      // get jump 
      char *cond_jump = get_cond_jump(LLVMGetICmpPredicate(LLVMGetOperand(instruction, 0)));
      // emit emit jxx L1 
      fprintf(fp,"\t%s\t%s\n", cond_jump, L1);
      // free mem 
      free(cond_jump);
      // get L2 
      char *L2 = bb_labels[LLVMValueAsBasicBlock(LLVMGetOperand(instruction, 1))];
      // emit jmp L2
      fprintf(fp,"\tjmp\t%s\n", L2);

    }
  }
  // ignore if Alloca
  else if (LLVMIsAAllocaInst(instruction)) return;

  // arithmetic (add/cmp/mul/sub)
  else {
    // x = register 
    char *X;
    // if A has physical 
    if (reg_map[instruction] != -1) {
      // set X
      X = get_reg(reg_map[instruction]);
    // else 
    } else {
      // X = %eax
      X = (char *)malloc(sizeof(char) * 4);
      strcpy(X, "eax");
    }
    // get A
    LLVMValueRef A = LLVMGetOperand(instruction, 0);
    // if A constant 
    if (LLVMIsAConstant(A)) {
        char *s = LLVMPrintValueToString(A);
        char *t = s + 4;
        // emit emit movl $A, X
        fprintf(fp,"\tmovl\t$%s, %%%s\n", t, X);
        LLVMDisposeMessage(s);
    // if temp 
    } else {
      // if physical 
      if (reg_map[A] != -1) {
          char *r = get_reg(reg_map[A]);
          // emit emit movl %eyy, X  (do not emit if both registers are the same)
          if (strcmp(r, X) != 0) fprintf(fp,"\tmovl\t%%%s, %%%s\n", r, X);
          free(r);
      // eske if in mem
      }
      else {
        // get offset 
        int k = offset_map[A] * -1;
        // emit movl n(%ebp), X
        fprintf(fp,"\tmovl\t%d(%%ebp), %%%s\n", k, X);
      }
    }
    // get B 
    LLVMValueRef B = LLVMGetOperand(instruction, 1);
    // get op code 
    char *op = get_arith_op(LLVMGetInstructionOpcode(instruction));
    // if B is a constant 
    if (LLVMIsAConstant(B)) {
      char *s = LLVMPrintValueToString(B);
      char *t = s + 4;
      // emit addl $B, X  [Note: you can replace addl by cmpl, subl or imull based on opcode]
      fprintf(fp,"\t%s\t$%s, %%%s\n", op, t, X);
      LLVMDisposeMessage(s);
    // if B is temp and has physical 
    } else if (reg_map[B] != -1) {
      char *r = get_reg(reg_map[B]);
      // emit emit addl %ezz, X
      fprintf(fp,"\t%s\t%%%s, %%%s\n", op, r, X);
      free(r);
    // if temp and does not  
    } else {
      // get off set 
      int k = offset_map[B] * -1;
      // emit addlm(%ebp), X
      fprintf(fp,"\t%s\t%d(%%ebp), %%%s\n", op, k, X);
    }
    // if A in mem 
    if (reg_map[instruction] == -1) {
      // get offset 
      int k = offset_map[instruction] * -1;
      // emit movl %eax, k(%ebp)
      fprintf(fp,"\tmovl\t%%eax, %d(%%ebp)\n", k);
    }
    // free X
    free(X);
    // free op code
    free(op);
  }
}


/**************** codegen() ****************/
/* see codegen.h for description */ 
void codegen(LLVMModuleRef m, FILE*fp) {
  // get register map 
  unordered_map<LLVMValueRef, int> reg_map = register_allocation(m);
  // iterate through each function 
  for (LLVMValueRef fn = LLVMGetFirstFunction(m); fn; fn = LLVMGetNextFunction(fn)) {
    // initialize bb label map 
    unordered_map<LLVMBasicBlockRef, char*> labels;
    // initialize offset map 
    unordered_map<LLVMValueRef, int> offset_map;
    // set ebx bool 
    bool ebx_isUsed = false;
    // iterate through reg map 
    printf("test1\n");
    //
    for (auto it = reg_map.begin(); it != reg_map.end(); it++) {
      // if ebx assigned
      if (it->second == 1) {
        // set used
        ebx_isUsed = true;
        // break 
        break;

      }
    }
    //test
    printf("test2\n");
    // fill offset and register map and get mem 
    int mem = get_offset_map(fn, offset_map, reg_map);
    printf("test3\n");
    // if no first block continue 
    if (LLVMGetFirstBasicBlock(fn) == NULL) continue;
    // print
    printf("creating labels\n");
    // set labels 
    labels = create_bb_labels(fn);
    // test
    printf("labels created\n");
    // call print directives 
    print_directives(fp, mem, ebx_isUsed);
    // iterate through each basic block 
    for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)) {
      // if not first bb 
      if (bb != LLVMGetFirstBasicBlock(fn)) {
        // print label as start 
        fprintf(fp,"%s:\n", labels[bb]);

      }
      // iterate through each instruction 
      for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; instruction = LLVMGetNextInstruction(instruction)) {
        // check instruction 
        check_instruction(instruction, labels, offset_map, reg_map, ebx_isUsed, fp);
      
      }
    }
    // iterate through each label to free memort 
    for (auto it = labels.begin(); it != labels.end(); it++) {
      // free memory 
      free(it->second);

    }
  }
}


int get_offset_map(LLVMValueRef fn, unordered_map<LLVMValueRef, int> &offset_map, unordered_map<LLVMValueRef, int> &reg_map) {
  // initialize memory position to 0 
  int mem = 0;
  // set parameter to null 
  LLVMValueRef param = NULL; 
  // iterate through bb
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)) {
    // iterate through instructions 
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; instruction = LLVMGetNextInstruction(instruction)) {
      // if store or argument 
      if (LLVMIsAStoreInst(instruction) && LLVMIsAArgument(LLVMGetOperand(instruction, 0))){
        // set param 
        param = LLVMGetOperand(instruction, 1);
        // break 
        break;
      }
    }
  }
  // iterate through basic block 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)) {
    // iterate through instruction 
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; instruction = LLVMGetNextInstruction(instruction)) {
      // if instruction = parameter 
      if (instruction == param) {
        //
        printf("test in get reg\n");
        // assign pos 
        offset_map[instruction] = -8;
        // continue to next
        continue;

      }
      // if alloca instruction 
      if (LLVMIsAAllocaInst(instruction)) {
        printf("test in get reg2\n");
        // add to memory pos 
        mem += LLVMGetAlignment(instruction);
        // assign position in offset
        offset_map[instruction] = mem;

      }
      // first operand 
      LLVMValueRef first_op = LLVMGetOperand(instruction, 0);
      // second operand 
      LLVMValueRef second_op = LLVMGetOperand(instruction, 1);
      // if no register 
      if (reg_map[instruction] == -1) {
        printf("test in get reg3\n");
          // if is a load 
          if (LLVMIsALoadInst(instruction)) {
            // assign pos in offset map 
            offset_map[instruction] = offset_map[first_op];

          }
          // if arithmetic 
          else if (LLVMIsAStoreInst(LLVMGetUser(LLVMGetFirstUse(instruction)))) {
            // get user of store instruction 
            LLVMValueRef u = (LLVMGetUser(LLVMGetFirstUse(instruction)));
            // if first op in memory 
            if (offset_map[LLVMGetOperand(u, 1)] == offset_map[first_op]){
              // assign instruction to mem 
              offset_map[instruction] = offset_map[first_op];
            }
            // if second in op 
            else if (offset_map[LLVMGetOperand(u, 1)] == offset_map[second_op]){
              // assign instruction to mem
              offset_map[instruction] = offset_map[second_op];
            }
          }
          // else put in mem 
          else {
            mem += 4;
            offset_map[instruction] = mem;
          }
        }
      }
  }
  printf("test in get reg4\n");
  // return pos 
  return mem;
}

/**************** create_bb_labels() ****************/
/* see codegen.h for description */ 
unordered_map<LLVMBasicBlockRef, char*> create_bb_labels(LLVMValueRef fn) {
  // initialize labels map 
  unordered_map<LLVMBasicBlockRef, char*> labels;
  // initialize bb count 
  int bb_count = 0;
  // iterate through bbs 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)) {
    // if first 
    if (bb_count == 0) {
      // allocate memory for name 
      char *n = (char *) malloc(sizeof(char) * 10);
      // copy into memory 
      strcpy(n, ".LFB0");
      // assign in map 
      labels[bb] = n;
    } else {
      // allocate mem for label 
      char *n = (char *) malloc(sizeof(char) * 10);
      // copy into n 
      sprintf(n, "%s%d", ".L", bb_count);
      // assign in map 
      labels[bb] = n;

    }
    // increment count 
    bb_count++;
  }
  // return label map
  return labels;

}

/**************** print_directives() ****************/
/* see codegen.h for description */ 
void print_directives(FILE*fp, int mem, bool ebx_isUsed) {
  // copy filename into string 
  char* s_file = (char*)malloc(sizeof(char) * strlen("code_gen.s")+1);
  // print starting directives 
  strcpy(s_file, "code_gen.s");
  fprintf(fp,"\t.file\t\"%s\"\n",s_file);
  fprintf(fp,"\t.text\n");
  fprintf(fp,"\t.globl\tfn\n");
  fprintf(fp,"\t.type\tfn, @fntion\n");
  fprintf(fp,"fn:\n");
  fprintf(fp,".LFB0:\n");
  fprintf(fp,"\tpushl\t%%ebp\n");
  fprintf(fp,"\tmovl\t%%esp, %%ebp\n");
  fprintf(fp,"\tsubl\t$%d, %%esp\n", mem);
  // if ebx used 
  if (ebx_isUsed) {
    fprintf(fp,"\tpushl\t%%ebx\n");

  }
}

/**************** get_reg() ****************/
/* see codegen.h for description */ 
char *get_reg(int reg_num) {
  // allocate mem for reg name 
  char *c_reg = (char *)malloc(sizeof(char) * 4);
  // if statements 
  if (reg_num == 0) {
    strcpy(c_reg, "ebx");
  }
  else if (reg_num == 1) {
    strcpy(c_reg, "ecx");
  }
  else if (reg_num == 2) {
    strcpy(c_reg, "edx");
  }
  else {
    strcpy(c_reg, "edx");
  }
  return c_reg;
}

/**************** print_function_end() ****************/
/* see codegen.h for description */ 
void print_function_end(FILE*fp, bool ebx_isUsed) {
  // if ebx is used 
  if (ebx_isUsed) {
    // pop off stack 
    fprintf(fp,"\tpopl\t%%ebx\n");

  }
  // leave
  fprintf(fp,"\tleave\n");
  // ret 
  fprintf(fp,"\tret\n");

}

/**************** get_cond_jump() ****************/
/* see codegen.h for description */ 
char *get_cond_jump(LLVMIntPredicate pred) {
  // allocate mem for cond jump (max = 3 letters)
  char *cond_jump = (char *)malloc(sizeof(char) * 4);
  switch (pred) {
    case LLVMIntEQ: {
        strcpy(cond_jump, "je");
        break;
    }
    case LLVMIntSGE: {
        strcpy(cond_jump, "jge");
        break;
    }
    case LLVMIntUGE: {
        strcpy(cond_jump, "jge");
        break;
    }
    case LLVMIntSLT: {
        strcpy(cond_jump, "jl");
        break;
    }
    case LLVMIntULT: {
        strcpy(cond_jump, "jl");
        break;
    }
    case LLVMIntSGT: {
        strcpy(cond_jump, "jg");
        break;
    }
    case LLVMIntUGT: {
        strcpy(cond_jump, "jg");
        break;
    }
    case LLVMIntSLE: {
        strcpy(cond_jump, "jle");
        break;
    }
    case LLVMIntULE: {
        strcpy(cond_jump, "jle");
        break;
    }
    default: {
        break;
    }
  }
  // return cond jump 
  return cond_jump;

}

/**************** get_arith_op() ****************/
/* see codegen.h for description */ 
char *get_arith_op(LLVMOpcode opcode) {
  // allocate for op code (max = 5) 
  char *op = (char *)malloc(sizeof(char) * 6);
  // switch 
  switch (opcode) {
    case LLVMAdd:
        strcpy(op, "addl");
        break;
    case LLVMSub:
        strcpy(op, "subl");
        break;
    case LLVMMul:
        strcpy(op, "imull");
        break;
    case LLVMICmp:
        strcpy(op, "cmpl");
        break;
    default: {
        break;
    }
  }
  // return 
  return op;

}








