LLVMCODE = llvm_parser
TEST = test

CFLAGS = g++ -Wall -Wextra  -g  `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/

all: $(LLVMCODE) optimizer.o llvm_file

$(LLVMCODE): $(LLVMCODE).c optimizer.o
	clang++ -g `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/ -c optimizer.o $(LLVMCODE).c -o $(LLVMCODE).o
	clang++ `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/  llvm_parser.o  optimizer.o -o llvm_parser



optimizer.o: optimizer.c optimizer.h
	$(CFLAGS) -c optimizer.c 


llvm_file: $(TEST).c
	clang -S -emit-llvm $(TEST).c -o $(TEST).ll


clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
