#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_BUF 1024

int N;

int main(int argc, char* argv[])
{
	int fd;
	char * myfifo;
    if (argc == 1){
      	myfifo = "/home/fudalinm/Desktop/masterslave/stream";
		N = 10;
    } else {
     	myfifo = argv[1];
	 N = atoi(argv[2]);		
    }
	
	//zbieramy swoje PID
	pid_t pid = getpid();
	
    char buf1[MAX_BUF];
	char buf2[MAX_BUF];
    fd = open(myfifo, O_WRONLY);

    while(N > 0){
		
		FILE *fp;
		fp = popen("date","r");
		//	%zbieramy swoje PID i dodajemy date
		fgets(buf1, MAX_BUF , fp);
		pclose(fp);	
		
		sprintf(buf2, "Slave: %d - %s", pid, buf1);
		//tutaj trzeba utworzyc buforek
		write(fd, buf2, sizeof(buf2));		
		
		N -= 1;
		sleep(2);
	}

    return 0;
}

