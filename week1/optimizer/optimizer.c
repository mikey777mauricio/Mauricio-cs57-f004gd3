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

std::unordered_set<LLVMValueRef> compute_gen(LLVMBasicBlockRef block){
 unordered_map<LLVMValueRef, LLVMValueRef> gen; 
 for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction; instruction = LLVMGetNextInstruction(instruction)){
   if (LLVMIsAStoreInst(instruction)){
     LLVMValueRef address = LLVMGetOperand(instruction, 1);
     gen[address] = instruction; 

   }
 }
 unordered_set<LLVMValueRef> gen_set; 
 for (auto it = gen.begin(); it != gen.end(); it++){
   if (it->second != NULL){
     gen_set.insert(it->second);

   }
 }
 return gen_set; 

}

// Computing the set KILL[B] of a basic block B
// Compute the set "S" of all store instructions in the
// given function before computing KILL for all basic blocks.
// Initialize KILL[B] = empty set
// for each instruction I in basic block B do the following:
// If I is a store instruction add all the instructions in S 
// that get killed by I to KILL[B] [Note: A instruction cannot kill itself]

std::unordered_set<LLVMValueRef> compute_set_s(LLVMValueRef fn){
  // comput set s of all store instructions in given function 
  std::unordered_set<LLVMValueRef> s_set;
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)){
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
      // add store instruction to set 
      if (LLVMIsAStoreInst(instruction)){
        s_set.insert(instruction);

      }  
    }
  }
  // return set 
  return s_set; 

}

std::unordered_set<LLVMValueRef> compute_kill(LLVMBasicBlockRef block, std::unordered_set<LLVMValueRef> s_set){
  std::unordered_set<LLVMValueRef> kill_set;
  
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
    if (LLVMIsAStoreInst(instruction)){
      LLVMValueRef address = LLVMGetOperand(instruction, 1);
      for (LLVMValueRef instructionB : s_set){
        if (instruction == instructionB) continue; 
        LLVMValueRef addressB = LLVMGetOperand(instructionB, 1);
        if (address == addressB) {
          kill_set.insert(instructionB);

        }
      }
    }

    
  }

  return kill_set; 

}


std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> compute_in_out(LLVMValueRef fn, unordered_set<LLVMValueRef> s){
  // map of in sets
  std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> in; 
  // map of out sets
  std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> out; 
  // set in of each bb to empty
  for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)){
    in[bb] = unordered_set<LLVMValueRef> ();

  }
  // set out of each bb to gen 
  for(LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)){
    out[bb] = compute_gen(bb);

  }
  // map of predecessors 
  unordered_map<LLVMBasicBlockRef, vector<LLVMBasicBlockRef>> predecessors;
  // iterate through each bb  
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)) {
    // get terminator 
    LLVMValueRef terminator = LLVMGetBasicBlockTerminator(bb);
    // get number of successors 
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
    for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)){
      // in set 
      unordered_set<LLVMValueRef> in_bb; 
      // for each bb in predecessors
      for (LLVMBasicBlockRef p : predecessors[bb]){
        // get insert union of all predecessors 
        in_bb.insert(out[p].begin(), out[p].end());

      }
      // set in to in 
      in[bb] = in_bb; 
      // old out = out[bb]
      unordered_set<LLVMValueRef> old = out[bb];
      // get in of bb to cmpute in - kill 
      unordered_set<LLVMValueRef> sub = in[bb];
      // compute in - kill 
      for (LLVMValueRef v : compute_kill(bb, s)){
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

bool constant_prop(LLVMValueRef fn, unordered_set<LLVMValueRef> s){
  unordered_map<LLVMBasicBlockRef, unordered_set<LLVMValueRef>> in = compute_in_out(fn, s);

  return check_loadInstructions(fn, in);

}

bool check_loadInstructions(LLVMValueRef fn, unordered_map<LLVMBasicBlockRef, unordered_set<LLVMValueRef>> in){
  bool changed = false; 
  for (LLVMBasicBlockRef bb = LLVMGetFirstBasicBlock(fn); bb; bb = LLVMGetNextBasicBlock(bb)){
    unordered_set<LLVMValueRef> r = in[bb];
    unordered_set<LLVMValueRef> to_delete = unordered_set<LLVMValueRef>(); 

    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction; instruction = LLVMGetNextInstruction(instruction)){
      if (LLVMIsAStoreInst(instruction)){
        unordered_set<LLVMValueRef>::iterator it = begin(r);
        while (it != end(r)){
          LLVMValueRef instructionB = *it; 
          if (LLVMGetOperand(instructionB, 0) == instruction){
            it = r.erase(it);
          } else {
            ++it; 
          }
        }
        r.insert(instruction);
      }
      if (LLVMIsALoadInst(instruction)){
        unordered_set<LLVMValueRef> loads = find_loads(instruction, r);
        if (loads.empty()) continue; 

        LLVMValueRef const_op = LLVMGetOperand(*(loads.begin()), 0);
        long constant = LLVMConstIntGetSExtValue(const_op);
        LLVMReplaceAllUsesWith(instruction, LLVMConstInt(LLVMInt32Type(), constant, 1));
        changed = true; 
        to_delete.insert(instruction); 

      }


    }
    for (LLVMValueRef i : to_delete){
      printf("const_prop deleteing\n");
      LLVMInstructionEraseFromParent(i);
    }
    to_delete = unordered_set<LLVMValueRef>(); 

  }
  return changed; 

}



/**************** optimize_mod() ****************/
/* see optimizer.h for description */
LLVMModuleRef optimize_mod(LLVMModuleRef m){
  /* function checks */
  bool sub_changed = true; 
  bool dead_changed = true; 
  bool const_changed = true;   
  bool const_prop = true; 
  // loop through functions 
  for (LLVMValueRef function =  LLVMGetFirstFunction(m); function != NULL; 
          function = LLVMGetNextFunction(function)) {     
    // loop through basic blocks 
    // reset bools 
    vector<char*> *slist;
	  slist = new vector<char*> ();
    
  
    for (LLVMBasicBlockRef block = LLVMGetFirstBasicBlock(function); block != NULL; 
            block = LLVMGetNextBasicBlock(block)){
      // vector to check seen instructions 
      
      // while either return true 
      sub_changed = true; 
      dead_changed = true; 
      const_changed = true;  
      const_prop = true; 
      unordered_set<LLVMValueRef> s = compute_set_s(function);
      while(sub_changed || dead_changed || const_changed) {
        // call each function 
        sub_changed = common_sub_expr(block, slist);
        dead_changed = dead_code_elimination(block);  
        
        
        constant_prop(function, s);
        const_changed = const_folding(block);
        

      } 
    }
  } 
  // return module
  return m; 
}