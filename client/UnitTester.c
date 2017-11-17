#include "file/file.h"
int main(int argc, char** argv){
    int tester;
    char* param;
    int fd;
    if(argc >1 ){
        tester = atoi(argv[1]);
        param = argv[2];
    }

    switch (tester){
        case 1:
            printf("\nSimulating socket sending using terminal\n\n");
            sendFile(param, 1);
            break;
        case 2:
            printf("Filesize of \"%s\": %i\n", param, fileSize(param));
            break;
        case 3:

            if((fd = open("test.txt", O_RDONLY)) < 0)
            {  fprintf(stderr, "Open error: test.txt\n");
                return -1; //send failed
            }
            recFile(fd);
            close(fd);
            break;
        default:
            fprintf(stderr, "No such switch%d\n", tester);


    }
}
