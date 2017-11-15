#include "file.h"


/* Description: finds size of file in bytes, writes to fd an unsigned int representing
 *              filesize, followed by filename, followed by the file's contents.
 * Accepts:     A char* representing the filename, and an int file descriptor to write to.
 * Returns:     1 on successful send, 0 on failure
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

/* Description:  Finds size of file in bytes, returns size.
 * Accepts:      Char* filename
 * Returns:      unsigned int representing file size.
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

/* Description:  Given a void* representing a socket file descriptor, reads file info and creates a file on the local disk.
 *               Reads/Writes from socket to file.
 *               Intended to be threaded.
 * Accepts:      Char* string to parse
 * Returns:      unsigned int representing file size.
 */
int recFile(int sd)//TODO not working
    {
//        char byte;
//        int x, fd;
//        char temp[64];
//        char temp2[64];
//        char *fname;
//        unsigned int fsize = 0;
//
//        bzero(temp, 64);
//        //Read 2 bytes for /f, then copy next 16 bits for unsigned file size/filename
//        for(x =0; x<10;x++) {
//            bzero( &byte, 1);
//            read(sd,&byte,1);
//            if(x>1) {
//                strcat(temp, &byte);
//            }
//        }
//        temp[8] = '\0';
//        //TODO not working correctly
//        fsize = atoi(temp);
//        printf("Debug-fname: %i", fsize);
//
//        //read filename
//        bzero(temp, 64);
//        for(x =0; x<8;x++) {
//            bzero( &byte, 1);
//            read(sd,&byte,1);
//            strcat(temp, &byte);
//
//        }
//        temp[8] = '\0';
//        //Attempts to create file
//        if((fd = open(temp, O_WRONLY | O_CREAT, 0666)) < 0)
//        {  fprintf(stderr, "Open error: %s\n", temp);
//            return 0; //write failed
//        }
//
//        //remaining bytes should be file contents
//        for(x= 0;x < fsize; x++) {
//            bzero( &byte, 1);
//            read(sd,&byte,1);
//            write(fd,&byte, 1);
//        }
//        close(fd);
//        return 1;    //write success
    }


