a.out : hw1.o validate.o testcase.o
	gcc hw1.o validate.o testcase.o -o a.out
hw1.o : hw1.c hw1.h
	gcc -c hw1.c
testcase.o : testcase.c validate.h hw1.h
	gcc -c testcase.c
clean :
	rm -f hw1.o testcase.o a.out hw1.txt
