#include "file.h"


/* Description: Given initial char* containing /f, finds size of file in bytes, 
 *				writes to sd an unsigned int representing filesize, 
 *				followed by filename, followed by the file's contents.
 * Accepts:     Initial char*, int file/socket descriptor.
 * Returns:     1 on successful send, 0 on failure
 */

int sendFile(char* initStr, int sd){
	//increment past "/f "
    char * ptr;
	if(initStr[2] == ' '){ptr = initStr+3;}
	else{ptr = initStr+2;}
    strip(ptr);
    if(strlen(ptr) > 8){
        fprintf(stderr,"Client: Filename is too long:\"%s\"\n", ptr);
        return 0; //send failed
    }

    int fd;
    unsigned int size;
    char buffer[256];


    if((fd = open(ptr, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Client: Open error: %s\n", ptr);
        return 0; //send failed
    }

    //Find size of file
    lseek(fd, 0, SEEK_END);
    size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);     //reset pointer to  beginning of file

    //write file info to socket
    //size = htonl(size); This is not working correctly
	dprintf(sd, "/f%4i", size);
	bzero(buffer, 256);
    strncpy(buffer,ptr, strlen(ptr));
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

/* Description:  Given a char* representing initial string containing /f, reads file info and creates a file on the local disk.
 *               Reads/Writes from socket to file.
 * Accepts:      Char* string to parse
 * Returns:      Int success/failure (1/0).
 */
int recFile(char * initStr, int sd)
    {

		//increment past "/f "
        char * ptr;
        if(initStr[2] == ' '){ptr = initStr+3;}
        else{ptr = initStr+2;}
		
        int x, fd;
        char temp[256];
        unsigned long fsize = 0;

        bzero(temp, 256);
		
		//retrieve filesize
        strncpy(temp, ptr, 4);
        ptr =  ptr+4;
        fsize= strtoul(temp, 0, 10);
		
		//TODO
        printf("Debug-fsize: %lu\n", fsize);

        //read filename
        bzero(temp, 256);
        if(*ptr != '\0'){ //already in temp
            strcpy(temp, ptr);
            ptr += strlen(temp);
        }
        else{
            read(sd, temp, 8);
        }

        //Attempts to create file
        if((fd = open(temp, O_WRONLY | O_CREAT, 0666)) < 0)
        {  fprintf(stderr, "\nClient: Open/Create error: %s\n", temp);
            return 0; //read failed
        }
        //TODO
        printf("Debug: Opened/created file %s for writing. \n", temp);

        if(ptr != '\0'){
            write(fd, ptr, strlen(ptr)); //write remaineder in temp
            fsize-= strlen(ptr);
        }
        //remaining bytes should be file contents
        x=0;
        ssize_t rd= 0;
        while(x < fsize) {
            bzero( &temp, 256);
            rd = read(sd,&temp,256);
            write(fd,&temp, strlen(temp));
            x+= 1+ rd;
        }

        //TODO
        printf("Debug: File Write Complete. \n");

        close(fd);
        return 1;    //write success
    }


/*
 * Takes in string and removes newline and tab characters
 */
void strip(char *toParse) {
    if (toParse != NULL) {
        int len = strlen(toParse);
        int i = 0;

        while (toParse[i] != '\0' && i < len) {
            if (toParse[i] == '\r')
                toParse[i] = '\0';

            else if (toParse[i] == '\n')
                toParse[i] = '\0';

            i++;

        }// end while
    }
}// end strip
