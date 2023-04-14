source = miniC
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

$(source).out: $(source).l $(source).y
	yacc -d -v  $(source).y
	lex $(source).l
	g++ -o $(source).out lex.yy.c y.tab.c ast.c

valgrind: ./miniC.out
	$(VALGRIND) -s ./miniC.out ./tests/p1.c

test: ./miniC.out
	./miniC.out ./tests/p1.c
	

clean:
	rm lex.yy.c y.tab.c y.tab.h $(source).out
