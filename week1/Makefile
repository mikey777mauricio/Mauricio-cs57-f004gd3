source = miniC
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
CC = g++

CFLAGS = -Wall -Wextra -g

all: $(source).out s_analyzer.o
	g++ -o $(source).out lex.yy.c y.tab.c ast.c s_analyzer.o


s_analyzer.o: s_analyzer.c s_analyzer.h
	$(CC) $(CFLAGS) -c s_analyzer.c



$(source).out: $(source).l $(source).y
	yacc -d -v  $(source).y
	lex $(source).l

	

valgrind: ./miniC.out
	$(VALGRIND) -s ./miniC.out ./tests/sample_miniC.c

test: ./miniC.out
	./miniC.out ./tests/p1.c
	

clean:
	rm lex.yy.c y.tab.c y.tab.h $(source).out s_analyzer.o y.output
