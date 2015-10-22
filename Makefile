
#Use GNU compiler
cc = gcc -g
CC = g++ -g

LEX=lex
YACC=yacc

all: shell cat_grep ctrl-c regular git-commit

tty-raw-mode.o: tty-raw-mode.c
	gcc -c tty-raw-mode.c

read-line.o: read-line.c
	gcc -c read-line.c

lex.yy.o: shell.l 
	$(LEX) shell.l
	$(CC) -c lex.yy.c

y.tab.o: shell.y
	$(YACC) -d shell.y
	$(CC) -c y.tab.c

command.o: command.cc
	$(CC) -c command.cc

shell:tty-raw-mode.o read-line.o  y.tab.o lex.yy.o command.o
	$(CC) -o shell tty-raw-mode.o read-line.o lex.yy.o y.tab.o command.o -lfl

cat_grep: cat_grep.cc
	$(CC) -o cat_grep cat_grep.cc

ctrl-c: ctrl-c.cc
	$(CC) -o ctrl-c ctrl-c.cc

regular: regular.cc
	$(CC) -o regular regular.cc

git-commit:
	git add *.h *.cc *.l *.y >> .local.git.out
	git commit -a -m "Commit Shell" >> .local.git.out

clean:
	rm -f lex.yy.c y.tab.c  y.tab.h shell ctrl-c regular cat_grep *.o

