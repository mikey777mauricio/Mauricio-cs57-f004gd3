#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

LLVMValueRef 

int main(int argc, const char* argc[])
{
  // create module 
  LLVMModuleRed mod = LLVMModuleCreateWithName("my_module");



}