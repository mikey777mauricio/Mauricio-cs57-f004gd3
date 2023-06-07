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
#include "optimizer.h"

/**************** functions ****************/

/**************** common_sub_expr() ****************/
/* see optimizer.h for description */ 
bool common_sub_expr(LLVMBasicBlockRef bb, vector<char*> *slist){
  // change bool 
  bool change = false;
  // iterate through instructions
  for (LLVMValueRef instrA = LLVMGetFirstInstruction(bb); instrA; instrA = LLVMGetNextInstruction(instrA)){
    // bool to check if seen 
    bool seen = false; 
    // iterate through vector 
    for (unsigned int i = 0; i < (*slist).size(); i++){
      // set currInst char*
      char* currInst = (*slist)[i];
      // compare with instrA
      if (strcmp(currInst, LLVMPrintValueToString(instrA)) == 0){
        // set seen to true
        seen = true;
        // print seen 
        printf("SEEN\n");

      }
      // if seen 
      if (seen){
        // break 
        break; 

      }
    }
    // if seen 
    if (seen){
      // continue
      continue; 
    }
    if (LLVMIsAAllocaInst(instrA)) continue;
    // Compare instrA with instructions that follow it in the basic block
    for (LLVMValueRef instrB = LLVMGetNextInstruction(instrA); instrB; instrB = LLVMGetNextInstruction(instrB)){
      // Check if instrA and instrB have the same opcode and operands
      if (LLVMGetInstructionOpcode(instrA) == LLVMGetInstructionOpcode(instrB) &&
        LLVMGetNumOperands(instrA) == LLVMGetNumOperands(instrB)) {
        // set candidate to true 
        bool isCandidate = true;
        // check operands
        for (int i = 0; i < LLVMGetNumOperands(instrA); i++){
          if (LLVMGetOperand(instrA, i) != LLVMGetOperand(instrB,i)){
            // set candidate to false
            isCandidate = false; 
            // break 
            break;

          }
        } 
        if (!LLVMIsALoadInst(instrA)){
          // print
          printf("replacing\n");
          // replace
          LLVMReplaceAllUsesWith(instrB, instrA);
          // add to vector 
          slist->push_back(LLVMPrintValueToString(instrA));
          // set change to true
          change = true; 
          continue; 

        }
        if (!isCandidate) continue; 
        bool loadBetween = false; 
        // Check if instrA and instrB are load instructions
        if (LLVMIsALoadInst(instrA) && LLVMIsALoadInst(instrB)){
          // get operands 
          LLVMValueRef op1 = LLVMGetOperand(instrA, 0);
          LLVMValueRef op2 = LLVMGetOperand(instrA, 1);
          // iterate through instructions inbetween 
          for (LLVMValueRef currInstr = LLVMGetNextInstruction(instrA); currInstr != instrB; currInstr = LLVMGetNextInstruction(currInstr)){
            // if store and same operands
            if (LLVMIsAStoreInst(currInstr) && (LLVMGetOperand(currInstr, 0) == op1 && LLVMGetOperand(currInstr, 1) == op2)){
              // cannot be a candidate 
              loadBetween = true;
              break;

            }
          }      
        }
        // if not candidate
        if (loadBetween){
          // break 
          break;

        }  
        // If instrA and instrB are candidates for elimination, replace instrB with instrA
        if (isCandidate){
          // print
          printf("replacing\n");
          // replace
          LLVMReplaceAllUsesWith(instrB, instrA);
          // add to vector 
          slist->push_back(LLVMPrintValueToString(instrA));
          // set change to true
          change = true; 

        }
      } 
    }
  }
  // return change
  return change;

}



/**************** dead_code_elimination() ****************/
/* see optimizer.h for description */
bool dead_code_elimination(LLVMBasicBlockRef block) {
  // set change to false 
  bool change = false; 
  // iterate through instructions 
  
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
      // check if instruction has side effects
      if (LLVMIsATerminatorInst(instruction) || LLVMIsAStoreInst(instruction) || LLVMIsAAllocaInst(instruction)) continue; 

      if (LLVMGetFirstUse(instruction) == NULL){
        printf("deleting\n");
        // erase from parent
       
        LLVMInstructionEraseFromParent(instruction);
      
        // set change to true
        change = true; 
        // continue 
        continue;

      }
      
    bool used = false; 
    for (LLVMValueRef instrB = LLVMGetNextInstruction(instruction); instrB; instrB = LLVMGetNextInstruction(instrB)){
      int num = LLVMGetNumOperands(instrB);
      for (int i = 0; i < num; i++){
        if (LLVMGetOperand(instrB, i) == instruction){
          used = true; 
          break; 
        }
      }
    
    }
    if (!used){
             printf("deleting\n");
        // erase from parent
       
        LLVMInstructionEraseFromParent(instruction);
      
        // set change to true
        change = true; 
        // continue 
        continue;


    }
      
      
    }
  
  return change;
}
 

/**************** const_folding() ****************/
/* see optimizer.h for description */
bool const_folding(LLVMBasicBlockRef block){
  // flag
  bool change = false;
  // loop through basic blocks
 
  
    // loop through functions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
      // get op
      LLVMOpcode op = LLVMGetInstructionOpcode(instruction);
      // check if add, sub, or mul Opcode 
      if (op == LLVMAdd || op == LLVMSub || op == LLVMMul){
        printf("Arithmetic\n");
        // get operands
        LLVMValueRef op1 = LLVMGetOperand(instruction, 0);
        LLVMValueRef op2 = LLVMGetOperand(instruction, 1);
        // print values 
        LLVMDumpValue(op1);
        printf("\n");
        LLVMDumpValue(op2);
        printf("\n");
        // if both operands are constant
        if (LLVMIsAConstant(op1) && LLVMIsAConstant(op2)){
          // set const_res to NULL
          printf("constants\n");
          // constant res 
          LLVMValueRef res; 
          // switch statements
          switch (op){
            // add
            case LLVMAdd: {
              // call const add
              res = LLVMConstAdd(op1, op2);
              break; 

            }
            // sub 
            case LLVMSub: {
              // call const sub 
              res = LLVMConstSub(op1, op2);
              break; 
              
            }
            // mul
            case LLVMMul: {
              // call const mul 
              res = LLVMConstMul(op1, op2);
              break; 
              
            }
            // default 
            default: 
              // break 
              break; 

          }
          // replace all uses
          LLVMReplaceAllUsesWith(instruction, res);
          // set change to true
          change = true; 
        
        }
      }
    }
  
  // return flag
  return change;

}
/**************** compute_gen() ****************/
/* see optimizer.h for description */
std::unordered_set<LLVMValueRef> compute_gen(LLVMBasicBlockRef block){
  // initialize gen map  
  unordered_map<LLVMValueRef, LLVMValueRef> gen; 
  // iterate through each instruction 
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction; 
          instruction = LLVMGetNextInstruction(instruction)){
    // if store instruction 
    if (LLVMIsAStoreInst(instruction)){
      // get address of store 
      LLVMValueRef address = LLVMGetOperand(instruction, 1);
      // add to gen map 
      gen[address] = instruction; 

    }
  }
  // initialize gen set 
  unordered_set<LLVMValueRef> gen_set; 
  // iterate through each instruction in gen map 
  for (auto it = gen.begin(); it != gen.end(); it++){
   // if instruction != NULL
   if (it->second != NULL){
    // add to gen set
    gen_set.insert(it->second);

   }
  }
  // return gen set
  return gen_set; 

}

/**************** compute_set_s() ****************/
/* see optimizer.h for description */
std::unordered_set<LLVMValueRef> compute_set_s(LLVMValueRef fn){
  // initialize store set 
  std::unordered_set<LLVMValueRef> s_set;
  // iterate through each basic block of function 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; 
          bb = LLVMGetNextBasicBlock(bb)){
    // iterate through each instruction 
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; 
            instruction = LLVMGetNextInstruction(instruction)){
      // add store instruction to set 
      if (LLVMIsAStoreInst(instruction)){
        // insert into store set 
        s_set.insert(instruction);

      }  
    }
  }
  // return set 
  return s_set; 

}

/**************** compute_kill() ****************/
/* see optimizer.h for description */
std::unordered_set<LLVMValueRef> compute_kill(LLVMBasicBlockRef block, std::unordered_set<LLVMValueRef> s_set){
  // initialize kill set
  std::unordered_set<LLVMValueRef> kill_set;
  // iterate through each instruction 
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
    // if store instruction 
    if (LLVMIsAStoreInst(instruction)){
      // get address 
      LLVMValueRef address = LLVMGetOperand(instruction, 1);
      // for instruction in store set 
      for (LLVMValueRef instructionB : s_set){
        // if same instruction continue 
        if (instruction == instructionB) continue; 
        // get address of instructionB 
        LLVMValueRef addressB = LLVMGetOperand(instructionB, 1);
        // if killed by instruction
        if (address == addressB) {
          // insert instruction B
          kill_set.insert(instructionB);

        }
      }
    }
  }
  // return kill set 
  return kill_set; 

}

/**************** const_compute_in_out() ****************/
/* see optimizer.h for description */
std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> compute_in_out(LLVMValueRef fn, unordered_set<LLVMValueRef> s){
  // map of in sets
  std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> in; 
  // map of out sets
  std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> out; 
  // iterate through each basic block 
  for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; 
          bb = LLVMGetNextBasicBlock(bb)){
    // set each IN[B] to empty set 
    in[bb] = unordered_set<LLVMValueRef> ();
    // set each OUT[B] to gen
    out[bb] = compute_gen(bb);

  }
  // map of predecessors 
  unordered_map<LLVMBasicBlockRef, vector<LLVMBasicBlockRef>> predecessors;
  // iterate through each bb  
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)) {
    // get terminator 
    LLVMValueRef terminator = LLVMGetBasicBlockTerminator(bb);
    // iteratoe through each successor 
    for (unsigned int i = 0; i < LLVMGetNumSuccessors(terminator); i++) {
      // add successor to pred map 
      LLVMBasicBlockRef successor = LLVMGetSuccessor(terminator, i);
      predecessors[successor].push_back(bb);
   
    }
  }
  // change bool 
  bool change = true; 
  // while change 
  while (change) {
    // set change to false 
    change = false; 
    // iterate through each bb
    for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; 
            bb = LLVMGetNextBasicBlock(bb)){
      // initialize in set 
      unordered_set<LLVMValueRef> in_bb; 
      // compute kill 
      unordered_set<LLVMValueRef> kill = compute_kill(bb, s);
      // for each bb in predecessors
      for (LLVMBasicBlockRef p : predecessors[bb]){
        // insert all the OUT[p]
        in_bb.insert(out[p].begin(), out[p].end());

      }
      // set in to in 
      in[bb] = in_bb; 
      // old out = out[bb] 
      unordered_set<LLVMValueRef> old = out[bb];
      // get in of bb to cmpute in - kill 
      unordered_set<LLVMValueRef> sub = in[bb];
      // compute in - kill *********
      for (LLVMValueRef v : kill){
        sub.erase(v);

      }
      // out = gen union (in - kill)
      out[bb] = compute_gen(bb);
      // union (in - kill)
      out[bb].insert(sub.begin(), sub.end());
      // if out changed
      if (out[bb] != old){
        // set change to true 
        change = true; 

      }
    }

  }
  return in; 

}
/**************** find_loads() ****************/
/* see optimizer.h for description */
unordered_set<LLVMValueRef> find_loads(LLVMValueRef instruction, unordered_set<LLVMValueRef> r){
  // set of stores that write to address
  unordered_set<LLVMValueRef> loads; 
  // get address
  LLVMValueRef address = LLVMGetOperand(instruction, 0);
  // for instruction in r 
  for (LLVMValueRef instructionB : r){
    // continue if same or store 
    if (instructionB == instruction) continue; 
    if (!LLVMIsAStoreInst(instructionB)) continue; 
    // get adress B
    LLVMValueRef addressB = LLVMGetOperand(instructionB, 1);
    // if adresses match, insert into loads
    if (address == addressB){
      loads.insert(instructionB);

    }

  }
  // if empty, return empty set
  if (loads.empty()) return unordered_set<LLVMValueRef>();
  // get first instruction 
  LLVMValueRef first = *(loads.begin());
  // get value 
  LLVMValueRef value = LLVMGetOperand(first, 0);
  // for instruction in loads 
  for (LLVMValueRef instructionB : loads){
    if (!LLVMIsAConstant(value) || LLVMGetOperand(instructionB, 0) != value){
      
      return unordered_set<LLVMValueRef>(); 
    }
  }
  // return loads
  return loads; 

}

/**************** constant_prop() ****************/
/* see optimizer.h for description */
bool constant_prop(LLVMValueRef fn, unordered_set<LLVMValueRef> s){
  // compute in 
  unordered_map<LLVMBasicBlockRef, unordered_set<LLVMValueRef>> in = compute_in_out(fn, s);
  // call check load instructions 
  return check_loadInstructions(fn, in);

}

/**************** check_loadInstructions() ****************/
/* see optimizer.h for description */
bool check_loadInstructions(LLVMValueRef fn, unordered_map<LLVMBasicBlockRef, unordered_set<LLVMValueRef>> in){
  // initialize change 
  bool changed = false; 
  // iterate through each basic block 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; 
          bb = LLVMGetNextBasicBlock(bb)){
    // set R to IN[B]
    unordered_set<LLVMValueRef> r = in[bb];
    // initialize to delete set 
    unordered_set<LLVMValueRef> to_delete = unordered_set<LLVMValueRef>(); 
    // iterate through each instruction 
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; 
            instruction = LLVMGetNextInstruction(instruction)){
      // if store instruction 
      if (LLVMIsAStoreInst(instruction)){
        // iterate through each instruction in R 
        unordered_set<LLVMValueRef>::iterator it = begin(r);
        // while not at end 
        while (end(r) != NULL){
          // get next instruction in R
          LLVMValueRef instructionB = *it; 
          // if killed by instruction 
          if (LLVMGetOperand(instructionB, 0) == instruction){
            // remove from R
            it = r.erase(it);

          } 
          // else 
          else {
            // increment iterator 
            ++it; 

          }
          // insert store instruction 
          r.insert(instruction);
        }
      }
      // if is a load instruction 
      if (LLVMIsALoadInst(instruction)){
        // find all loads 
        unordered_set<LLVMValueRef> loads = find_loads(instruction, r);
        // if empty 
        if (loads.empty()){
          // continue 
          continue; 

        }
        // get constant 
        LLVMValueRef const_op = LLVMGetOperand(*(loads.begin()), 0);
        long constant = LLVMConstIntGetSExtValue(const_op);
        // replace all uses with constant in store 
        LLVMReplaceAllUsesWith(instruction, LLVMConstInt(LLVMInt32Type(), constant, 1));
        // set flag to true 
        changed = true; 
        // add load to delete
        to_delete.insert(instruction); 

      }
    }
    // for each instruction in to delete
    for (LLVMValueRef i : to_delete){
      // ersae from parent
      printf("const_prop deleteing\n");
      LLVMInstructionEraseFromParent(i);

    }
    // reset to delete 
    to_delete = unordered_set<LLVMValueRef>(); 

  }
  // return flag 
  return changed; 

}



/**************** optimize_mod() ****************/
/* see optimizer.h for description */
LLVMModuleRef optimize_mod(LLVMModuleRef m){
  /* function checks */
  bool sub_changed = true; 
  bool dead_changed = true; 
  bool const_changed = true;   
  // loop through functions 
  for (LLVMValueRef function =  LLVMGetFirstFunction(m); function != NULL; 
          function = LLVMGetNextFunction(function)) {     
    // vector to check seen instructions 
    vector<char*> *slist;
	  slist = new vector<char*> ();  
    // call constant prop 
    unordered_set<LLVMValueRef> s = compute_set_s(function);
    constant_prop(function, s);
    // loop through basic blocks 
    for (LLVMBasicBlockRef block = LLVMGetFirstBasicBlock(function); block != NULL; 
            block = LLVMGetNextBasicBlock(block)){
      // set all booleans 
      sub_changed = true; 
      dead_changed = true; 
      const_changed = true;  
      // while either return true 
      while(sub_changed || dead_changed || const_changed) {
        // call each function 
        sub_changed = common_sub_expr(block, slist);
        dead_changed = dead_code_elimination(block);  
        const_changed = const_folding(block);
        
      } 
    }
  } 
  // return module
  return m; 
}