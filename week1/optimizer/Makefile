LLVMCODE = llvm_parser
TEST = test

CFLAGS = clang -g `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/

all: $(LLVMCODE) optimizer.o llvm_file

$(LLVMCODE): $(LLVMCODE).c optimizer.o
	clang -g `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/ -c $(LLVMCODE).c -o $(LLVMCODE).o
	clang++ `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ $(LLVMCODE).o optimizer.o -o $@



optimizer.o: optimizer.c optimizer.h
	$(CFLAGS) -c optimizer.c -o optimizer.o


llvm_file: $(TEST).c
	clang -S -emit-llvm $(TEST).c -o $(TEST).ll


clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
