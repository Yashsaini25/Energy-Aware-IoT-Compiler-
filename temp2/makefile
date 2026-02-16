CC = gcc
CFLAGS = -g
LDFLAGS = -L/opt/homebrew/opt/flex/lib

all: compiler

compiler: parser.tab.c lex.yy.c ast.o energy.o optimizer.o main.o
	$(CC) $(CFLAGS) -o compiler parser.tab.c lex.yy.c ast.o energy.o optimizer.o main.o $(LDFLAGS) -lfl

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

energy.o: energy.c energy.h
	$(CC) $(CFLAGS) -c energy.c

optimizer.o: optimizer.c optimizer.h
	$(CC) $(CFLAGS) -c optimizer.c

main.o: main.c ast.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o parser.tab.* lex.yy.c compiler

test: compiler
	./compiler tests/test1.c