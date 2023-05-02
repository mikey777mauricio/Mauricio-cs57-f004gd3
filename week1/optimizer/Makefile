LLVMCODE = llvm_parser
TEST = test

CFLAGS = clang -g `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/ -c



optimizer.o: optimizer.c $(LLVMCODE).o
	$(CFLAGS) optimizer.c
	clang++ `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ $(LLVMCODE).o -o $@

$(LLVMCODE): $(LLVMCODE).c
	clang -g `llvm-config-15 --cflags` -I /usr/include/llvm-c-15/ -c $(LLVMCODE).c
	clang++ `llvm-config-15 --cxxflags --ldflags --libs core` -I /usr/include/llvm-c-15/ $(LLVMCODE).o -o $@

llvm_file: $(TEST).c
	clang -S -emit-llvm $(TEST).c -o $(TEST).ll


clean: 
	rm -rf $(TEST).ll
	rm -rf $(LLVMCODE)
	rm -rf *.o
