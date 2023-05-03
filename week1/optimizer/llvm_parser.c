#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "optimizer.h"

#define prt(x) if(x) { printf("%s\n", x); }

LLVMModuleRef createLLVMModel(char * filename){
	char *err = 0;
	LLVMMemoryBufferRef ll_f = 0;
	LLVMModuleRef m = 0;

	LLVMCreateMemoryBufferWithContentsOfFile(filename, &ll_f, &err);

	if (err != NULL) { 
		prt(err);
		return NULL;
	}
	
	LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);

	if (err != NULL) {
		prt(err);
	}

	return m;
}

void walkBBInstructions(LLVMBasicBlockRef bb){
	for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
  				instruction = LLVMGetNextInstruction(instruction)) {
	
		//LLVMGetInstructionOpcode gives you LLVMOpcode that is a enum		
		//LLVMOpcode op = LLVMGetInstructionOpcode(instruction);
        	
		//if (op == LLVMCall) //Type of instruction can be checked by checking op
		//if (LLVMIsACallInst(instruction)) //Type of instruction can be checked by using macro defined IsA functions
		//{
		

	}

}


void walkBasicblocks(LLVMValueRef function){
	for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
 			 basicBlock;
  			 basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
		
		printf("\nIn basic block\n");
		walkBBInstructions(basicBlock);
	
	}
}

void walkFunctions(LLVMModuleRef module){
	for (LLVMValueRef function =  LLVMGetFirstFunction(module); 
			function; 
			function = LLVMGetNextFunction(function)) {

		const char* funcName = LLVMGetValueName(function);	

		printf("\nFunction Name: %s\n", funcName);

		walkBasicblocks(function);
 	}
}

void walkGlobalValues(LLVMModuleRef module){
	for (LLVMValueRef gVal =  LLVMGetFirstGlobal(module);
                        gVal;
                        gVal = LLVMGetNextGlobal(gVal)) {

                const char* gName = LLVMGetValueName(gVal);
                printf("Global variable name: %s\n", gName);
        }
}

int main(int argc, char** argv)
{
	LLVMModuleRef m;
  LLVMModuleRef n; 

	if (argc == 2){
		m = createLLVMModel(argv[1]);
    n = optimize_mod(m);
	}
	else{
		m = NULL;
		return 1;
	}

	if (n != NULL){

    char** error; 
    LLVMPrintModuleToFile(n, "my_test.ll", error);


	}
	else {
	    printf("m is NULL\n");
	}

	
	return 0;
}
