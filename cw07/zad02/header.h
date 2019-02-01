//
// Created by fudalinm on 13.05.18.
//

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>




#ifndef UNTITLED_HEADER_H
#define UNTITLED_HEADER_H
#define PROJ 21

#define sPATH "myregion1"
#define bPATH "myregion2"
#define fPATH "myregion3"
#define cPATH "myregion4"


//const char *sPATH;

//const char *bPATH ;
//const char *fPATH;
//const char *cPATH;


/*
#define BarberSEM 0 // 0 w semaforze oznacza ze nie spi 1 spi
#define FifoSEM 1   //
#define CheckSEM 2
*/
typedef struct {
    int max;
    int head;
    int tail;
    pid_t queue[512];
    pid_t chair;
}fifo_t;

int pushFIFO(fifo_t* fifo ,pid_t pid);
pid_t popFIFO(fifo_t* fifo);
int isEMPTY(fifo_t* fifo);
int isFULL(fifo_t* fifo);
int findInQueue(fifo_t *fifo,pid_t pid);
int initFIFO(int size,fifo_t* toRET);
void printMicro(void);


#endif //UNTITLED_HEADER_H
