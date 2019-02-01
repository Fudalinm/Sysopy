#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_BUF 1024

char * myfifo;

void handler(int signal){
	
	/* remove the FIFO */
	unlink(myfifo);
	 exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
	
	signal(SIGTSTP,handler);
	signal(SIGINT,handler);
		
	int fd;
    if (argc == 1){
        myfifo = "/home/fudalinm/Desktop/masterslave/stream";
    } else {
     	myfifo = argv[1];
    }
	char buf[MAX_BUF];
	mkfifo(myfifo, S_IWUSR | S_IRUSR);
    fd = open(myfifo, O_RDONLY);
	
    while(1){
		int k = read(fd, buf, MAX_BUF);
		if (k == 0) {continue;}
		printf("Received: %s\n", buf);
	}

    return 0;
}

