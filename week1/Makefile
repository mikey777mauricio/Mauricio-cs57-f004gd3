source = miniC
$(source).out: $(source).l $(source).y
	yacc -d $(source).y
	lex $(source).l
	gcc -o $(source).out lex.yy.c y.tab.c

clean:
	rm lex.yy.c y.tab.c y.tab.h $(source).out