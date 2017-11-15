CC=gcc

tester:	    UnitTester.c file.o
	gcc -g UnitTester.c file.c -o tester
	
file.o:	file.c file.h
	gcc -g -c file.c

clean: 
	rm *.o tester
	