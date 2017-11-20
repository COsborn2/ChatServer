FLAGS= -g -c -m32

EXE = client
MAIN = ./client.c

FILE = ./file/file.o

${EXE}: ${FILE} ${MAIN}
	gcc -o ${EXE} ${MAIN} ${FILE} -lpthread

#=======================================================================================================================

file.o:	./file/file.c ./file/file.h
	gcc  ${FLAGS} ./file/file.c

#=======================================================================================================================

clean:
	rm file/*.o
	rm ${EXE}
