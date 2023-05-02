#include <llvm-c/Core.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"


int main(astNode* root)
{
  LLVMModuleRef mod = LLVMModuleCreateWithName("");
	LLVMSetTarget(mod, "x86_64-pc-linux-gnu");

  



}

LLVMValueRef func(LLVMModuleRef mod, astNode* funcNode){
  LLVMTypeRef param_type; 
  LLVMTypeRef ret_type;
  if (funcNode->func.param != NULL){
    param = LLVMInt32Type(); 
    ret_type = LLVMFunctionType(LLVMInt32Type(), param, 1, 0);
  } else {
    ret_type = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);

  }
  LLVMValueRef func = LLVMAddFunction(mod, "test", ret_type);
  
  LLVMBasicBlockRef first = LLVMAppendBasicBlock(func, "");

  LLVMBuildeRef builder = LLVMCreateBuilder(); 
  LLVMPositionBuilderAtEnd(builder,first);

  astNode* func_body = funcNode->func.body; 
  instructionHelper(mod, func_body, builder);
  

  
}


void basicBlockHelper(LLVMModuleRef mod, astStmt funcBody, LLVMBasicBlockRef first){

 
  vector<astNode*> slist = *(stmt->block.stmt_list);
		vector<astNode*>::iterator it = slist.begin();
		while (it != slist.end()){
		LLVMValue ref n = instructionHelper(mod, *it, first);
		it++;
	}
  



}

LLVMValueRef instructionHelper(LLVMModuleRef mod, astNode* node, LLVMBuilderRef builder){
  assert(node != NULL);
  switch (node->type)
  {
  case ast_stmt:{
    astStmt = node->stmt; 
    // call stmt helper 
    break;
  }
    

  case ast_rexpr:{
    astNode* lhs = node->rexpr.lhs; 
    astNode* rhs = node->rexpr.rhs; 

    // call stmt helper 
    break;
  }
    

  case ast_bexpr:{
    astNode* lhs = node->rexpr.lhs; 
    astNode* rhs = node->rexpr.rhs; 
    // call stmt helper 
    break;

  }
  
  case ast_uexpr: {
		astNode* expr = node->uexpr.expr; 

		break;
	}
  
  default:
    break;
  }


}

void stmt_helper(LLVMModuleRef mod, astStmt* node, LLVMValueRef func, LLVMBuilderRef builder){
	assert(node != NULL && node->type == ast_stmt);
	
	switch(node->stmt.type){
		case ast_call: 
						break;
		case ast_ret: 
						break;
		case ast_block:
						break;
		case ast_while:
						break;
		case ast_if:
      
			
			break;
		case ast_asgn:
      astNode* lhs = node->stmt.asgn.lhs;
      astNode* rhs = node->stmt.asgn.rhs; 
      char* var_name = lhs->var.name; 
      // generate code to get RHS 
      

      // generate LLVM code for assignment operation 

			break;
		case ast_decl:
      char* var_name = node->stmt.decl.name; 
      LLVMRef n = LLVMBuildAlloca(builder, LLVMInt32Type(), var_name);
      LLVMSetAlignment(n, 4);


      break;	
			
		default: {
					fprintf(stderr,"Incorrect node type\n");
				 	exit(1);
				 };
	}
}

// helper 

LLVMValueRef createBinaryOp(LLVMBuilderRef, char op, LLVMValueRef lhs, LLVMValueRef rhs) {
  LLVMValueRef res; 
  switch (op) {
    case '+': 
      res = LLVMBuildAdd(builder, lhs, rhs, "add");
      break;
    
    case '-': 
      res = LLVMBuildSub(builder, lhs, rhs, "sub");
      break; 
    
    case '*':
      res = LLVMBuildMul(builder, lhs, rhs, "mul");
      break;

    case '/':
      res = LLVMBuildSDiv(builder, lhs, rhs, "div");
      break; 

    default: 
      break; 
      
    
  }

  return res; 

}