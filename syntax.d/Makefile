parser: syntax.o syntaxtree.o main.c
	gcc syntax.o syntaxtree.o main.c -lfl -o parser

syntax.o: syntax.tab.c syntax.tab.h lex.yy.c
	gcc -c syntax.tab.c -o syntax.o

syntaxtree.o: syntaxtree.c syntaxtree.h
	gcc -c syntaxtree.c -o syntaxtree.o

syntax.tab.c: syntax.y
	bison -d syntax.y

lex.yy.c: lexical.l
	flex lexical.l

clean:
	rm -f syntax.o syntaxtree.o parser syntax.tab.c syntax.tab.h lex.yy.c
