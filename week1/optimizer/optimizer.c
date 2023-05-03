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


// LLVMOpcode LLVMGetInstructionOpcode(LLVMValueRef Inst)
// common sub-expression elimination function 
// LLVMReplaceAllUsesWith 
// LLVMValueRef LLVMGetOperand(LLVMValueRef Val, unsigned Index)
// saftey check only required when A and B are load instructions 
bool common_sub_expr(LLVMBasicBlockRef bb){
  bool change = false; 
  // for loop to iterate through instructions 
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction != NULL;
  				instruction = LLVMGetNextInstruction(instruction)) {
    // get Opcode	
		LLVMOpcode op = LLVMGetInstructionOpcode(instruction);
    // get operands
    LLVMValueRef op1 = LLVMGetOperand(instruction, 0);
    LLVMValueRef op2 = LLVMGetOperand(instruction, 1);
    LLVMValueRef next = LLVMGetNextInstruction(instruction);
    // loop through next instructions 
    while (next != NULL) {
      // get Opcode
      LLVMOpcode next_op = LLVMGetInstructionOpcode(next);
      // get operands
      LLVMValueRef next_op1 = LLVMGetOperand(next, 0);
      LLVMValueRef next_op2 = LLVMGetOperand(next, 1);
      // saftey check 
      if (LLVMIsALoadInst(instruction) && op1 == next_op1 && op2 == next_op2 && LLVMIsAStoreInst(next)){
        // break 
        break; 

      }
      // check if same instruction 
      if (LLVMGetInstructionOpcode(next) == op && LLVMGetOperand(next, 0) == op1 
              && LLVMGetOperand(next, 1) == op2){
        // replace all uses with instruction 
        printf("Replacing\n");
        LLVMReplaceAllUsesWith(next, instruction);
        // change flag to true
        change = true; 
        
      }
      next = LLVMGetNextInstruction(next);
    }
  }
  // return flag
  return change; 

}

bool dead_code_elimination(LLVMValueRef function){
  // flag 
  bool change = false; 
  // iloop through basic blocks
  for (LLVMBasicBlockRef block = LLVMGetFirstBasicBlock(function); block != NULL; 
          block = LLVMGetNextBasicBlock(block)){
    // loop thropugh instructions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
      // get first instruction 
     
      if (LLVMGetFirstUse(instruction) == NULL) {
        // get Opcode
        LLVMOpcode op = LLVMGetInstructionOpcode(instruction);
        // check if store or call 
        if (!LLVMIsAStoreInst(instruction) && !LLVMIsACallInst(instruction) && !LLVMIsABranchInst(instruction) && !LLVMIsAAllocaInst(instruction) && !LLVMIsAReturnInst(instruction)){
          // erase instruction if passes check 
          printf("deleting\n");
          LLVMInstructionEraseFromParent(instruction);
          // set change to true
          change = true; 

        }
       
      }

    }
  }

  return change; 

}
//   LLVMAdd            = 8,
  // LLVMFAdd           = 9,
  // LLVMSub            = 10,
  // LLVMFSub           = 11,
  // LLVMMul            = 12,
  // LLVMFMul           = 13,
  // LLVMUDiv           = 14,
  // LLVMSDiv           = 15,
  // LLVMFDiv           = 16,
  // LLVMURem           = 17,
  // LLVMSRem           = 18,
  // LLVMFRem           = 19,
  // LLVMConstantIntValueKind, LLVMValueKind
bool const_folding(LLVMValueRef function){
  // flag
  bool change = false;
  // loop through basic blocks
  for (LLVMBasicBlockRef block = LLVMGetFirstBasicBlock(function); block != NULL; 
          block = LLVMGetNextBasicBlock(block)){
    // loop through functions
    for (LLVMValueRef instruction = LLVMGetFirstInstruction(block); instruction != NULL; 
            instruction = LLVMGetNextInstruction(instruction)){
      // get op
      LLVMOpcode op = LLVMGetInstructionOpcode(instruction);
      // check if add, sub, or mul Opcode 
      if (op == LLVMAdd || op == LLVMSub || op == LLVMMul){
        // get operands
        LLVMValueRef op1 = LLVMGetOperand(instruction, 0);
        LLVMValueRef op2 = LLVMGetOperand(instruction, 1);
        // if both operands are constant
        if (LLVMIsConstant(op1) && LLVMIsConstant(op2)){
          // set const_res to NULL
          LLVMValueRef const_res = NULL; 
          // switch statement 
          switch (op) {
            // add case
            case LLVMAdd: {
              const_res = LLVMConstAdd(op1, op2);
              break;

            }
            // sub case
            case LLVMSub: {
              const_res = LLVMConstSub(op1, op2);
              break;

            }
            // mult case 
            case LLVMMul: {
              const_res = LLVMConstMul(op1, op2);
              break;

            }
            // default
            default:
              break; 
          }
          // check const res
          if (const_res != NULL) {
            // replace all uses with const
            LLVMReplaceAllUsesWith(instruction, const_res);
            // erase instruction 
            //LLVMInstructionEraseFromParent(instruction);
            // set change to true
            change = true; 

          }
        }
      }
    }
  }
  // return flag
  return change;

}


LLVMModuleRef optimize_mod(LLVMModuleRef m){
  /* common sub-expression elimination */
  // loop through functions
    printf("common sub\n");
  int changed = 0; 
  while (changed == 0) {
  for (LLVMValueRef function =  LLVMGetFirstFunction(m); function != NULL; 
        function = LLVMGetNextFunction(function)) {
      // loop through basic blocks
      for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function); basicBlock != NULL;
            basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
          // set changed 
          
          if (!common_sub_expr(basicBlock)) {
            changed = 1; 
          } else {
            changed = 0; 
          }
      }
  }
  /* dead code elimination */
  // loop through functions
  for (LLVMValueRef function =  LLVMGetFirstFunction(m); function; 
          function = LLVMGetNextFunction(function)) {
    // set changed
    
    // check if change is true
   
      // call function until change is false
    if (!dead_code_elimination(function) && changed == 1){
      changed = 1; 
   
    } else {
      changed = 0; 
    }

    
  }
  /* constant folding */
  // loop through functions
  for (LLVMValueRef function =  LLVMGetFirstFunction(m); function; 
          function = LLVMGetNextFunction(function)) {
    // set changed          
   
    // check if change is true
    
      // call function until change is false
     if (!const_folding(function) && changed == 1){
       changed = 1;
     } else {
       changed = 0; 
     }

    
  }
  }
  // return module
  return m; 

}

