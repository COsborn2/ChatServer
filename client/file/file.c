#include "file.h"


/* Accepts: a char* representing the filename, and a file descriptor to write to.
 * Returns: 1 on successful send, 0 on failure
 */

int sendFile(char* fileName, int sd){

    if(strlen(fileName) > 8){
        fprintf(stderr,"Filename is too long:%s\n", fileName);
        return 0; //send failed
    }

    int fd;
    unsigned int size;
    char byte;

    if((fd = open(fileName, O_RDONLY)) < 0)
    {  fprintf(stderr, "Open error: %s\n", fileName);
        return 0; //send failed
    }

    //Find size of file
    lseek(fd, 0, SEEK_END);
    size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);     //reset pointer to  beginning of file

    //write file info to socket
    write(sd, &size, sizeof(size));
    write(sd, fileName, strlen(fileName));

    //reads until eof, writes bit by bit to client
    while(read(fd,&byte,1)){
        write(sd, &byte, 1);
    }
    close(fd);
    return 1;
}

/*Accepts: Char* filename
 *Returns: unsigned int representing file size.
 */
unsigned int fileSize(char* fileName){
    unsigned int size;
    int fd;
    if((fd = open(fileName, O_RDONLY)) < 0)
    {  fprintf(stderr, "Open error: %s\n", fileName);
        return -1; //send failed
    }

    //Find size of file
    lseek(fd, 0, SEEK_END);
    size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);     //reset pointer to  beginning of file

    close(fd);
    return size;
}
