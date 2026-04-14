# Makefile for Energy-Aware Compiler (Correct Flow)
CC = gcc
CFLAGS = -g -Wall

# Object files
OBJS = parser.tab.o lex.yy.o ast.o tac.o tac_optimizer.o energy.o main.o

all: compiler

compiler: $(OBJS)
	$(CC) $(CFLAGS) -o compiler $(OBJS)

# Parser generation
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

# Lexer generation
lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

# Object file compilation
parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

tac.o: tac.c tac.h
	$(CC) $(CFLAGS) -c tac.c

tac_optimizer.o: tac_optimizer.c tac_optimizer.h
	$(CC) $(CFLAGS) -c tac_optimizer.c

energy.o: energy.c energy.h
	$(CC) $(CFLAGS) -c energy.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o parser.tab.* lex.yy.c compiler

test: compiler
	@echo "=== Testing Correct Compiler Flow ==="
	@echo ""
	@echo "Test 1: Basic Optimization"
	./compiler ../tests/test1.c
	@echo ""
	@echo "Test 2: Constant Folding"
	./compiler ../tests/test_constant.c

.PHONY: all clean test