#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include "llvm_parser.c"
#include "optimizer.c"


int main (int argc, char* argv[]) {

  LLVMModuleRef m = createLLVMModel(argv[1]);
  LLVMModuleRef n = optimize(m);
  
}