FLAGS= -g -c -m32 -pthreads

EXE = client
MAIN = ./client.c

FILE = ./file/file.o

${EXE}: ${FILE} ${MAIN}
	gcc -o ${EXE} ${MAIN} ${FILE}

#=======================================================================================================================

file.o:	./file/file.c ./file/file.h
	gcc  ${FLAGS} ./file/file.c

#=======================================================================================================================

clean:
	rm file/*.o
	rm ${EXE}
