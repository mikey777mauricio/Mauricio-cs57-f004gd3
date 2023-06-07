LLVMCODE = llvm_parser
TEST = test

CFLAGS = g++ -Wall -Wextra  -g  `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/

all: $(LLVMCODE) optimizer.o codegen.o

$(LLVMCODE): $(LLVMCODE).c optimizer.o codegen.o
	clang++ -g `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/ -c optimizer.o codegen.o $(LLVMCODE).c -o $(LLVMCODE).o
	clang++ `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/  llvm_parser.o  optimizer.o codegen.o -o llvm_parser


codegen.o: codegen.c codegen.h 
	$(CFLAGS) -c codegen.c 


optimizer.o: optimizer.c optimizer.h
	$(CFLAGS) -c optimizer.c 





clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
