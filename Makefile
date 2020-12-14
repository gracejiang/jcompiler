
jc: main.o token.o asm_writer.o stack.o
	clang -Wall main.o token.o asm_writer.o stack.o -o jc

main.o: main.c
	clang -Wall -c main.c

token.o: token.c token.h
	clang -Wall -c token.c

asm_writer.o: asm_writer.c asm_writer.h
	clang -Wall -c asm_writer.c

stack.o: stack.c
	clang -Wall -c stack.c

clean:
	rm jc *.o