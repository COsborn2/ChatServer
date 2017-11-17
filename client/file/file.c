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
    char buffer[256];


    if((fd = open(fileName, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Open error: %s\n", fileName);
        return 0; //send failed
    }

    //Find size of file
    lseek(fd, 0, SEEK_END);
    size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);     //reset pointer to  beginning of file

    //write file info to socket
    size = htonl(size);
    //write(sd, &size, sizeof(size));
    dprintf(sd, "%4i", size);

    sprintf(buffer, "%s", fileName);
    write(sd, &buffer, 8);
    bzero(buffer, 256);
    //reads until eof, writes to client
    while(read(fd,&buffer,256)){
        write(sd, &buffer, 256);
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
int recFile(int sd)
    {
        int x, fd;
        char temp[256];
        unsigned long fsize = 0;

        bzero(temp, 256);
        //Read 2 bytes for /f
        read(sd, temp, 2);
        bzero(temp, 256);

        read(sd, &temp, 4);
        fsize= strtoul(temp, 0, 10);
        printf("Debug-fname: %i", fsize);

        //read filename
        bzero(temp, 256);
        read(sd, temp, 8);
        temp[8] = '\0';
        //Attempts to create file
        if((fd = open(temp, O_WRONLY | O_CREAT, 0666)) < 0)
        {  fprintf(stderr, "\nOpen/Create error: %s\n", temp);
            return 0; //read failed
        }

        //remaining bytes should be file contents
        x=0;
        int test;
        while(x < fsize) {
            bzero( &temp, 256);
            test = read(sd,&temp,256);
            temp[test]='\0';
            write(fd,&temp, 256);
            x= x + 256;
        }
        close(fd);
        return 1;    //write success
    }


