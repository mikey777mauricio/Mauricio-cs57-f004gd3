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
      // bool for indirect uses
      bool isUsed = false; 
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