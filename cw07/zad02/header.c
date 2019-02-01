//
// Created by fudalinm on 13.05.18.
//


#include "header.h"

int initFIFO(int size,fifo_t* toRET){
    toRET->tail = 0;
    toRET->head = -1;
    toRET->max  = size;
    toRET->chair = 0;

    return 1;
}


int pushFIFO(fifo_t* fifo ,pid_t pid){
    //musimy tylko sprawdzic czy jest pelna
    if (isFULL(fifo) == 1) {return -1;}
    if (isEMPTY(fifo) == 1) {fifo->head = 0; fifo->tail = 0;}

    fifo->queue[fifo->tail++] = pid;

    if (fifo->tail == fifo->max) {fifo->tail = 0;}

    return 1;
}

pid_t popFIFO(fifo_t* fifo){
    //musimy sprawdzic czy jest pusta
    if (isEMPTY(fifo) == 1) {return -1;}
    fifo->chair  = fifo->queue[fifo->head++];

    //jesli head wyszedl poza max
    if(fifo->head == fifo->max) { fifo->head = 0; }

    //jesli nam sie zamknal head i tail
    if(fifo->head == fifo->tail) {fifo->head = -1;}

    return  fifo->chair ;
}


int isEMPTY(fifo_t* fifo){
    if ( fifo->head == -1 ) {return 1;}
    return -1;
}

int findInQueue(fifo_t *fifo,pid_t pid){ //1 jesi jest w kolejce -1 jesli nie ma

    if (isEMPTY(fifo) == 1) {return -1;}
    int start = fifo->head;
    int end   = fifo->tail;

    if(fifo->chair == pid){
        return 1;
    }

    while(start != end){

        if(fifo->queue[start] == pid){
            return 1;
        }

        start ++;
        if(start == fifo->max){
            start = 0;
        }
    }

    return -1;
}


int isFULL(fifo_t* fifo){
    if(fifo->head == fifo->tail) {return 1;}
    return -1;
}

void printMicro(void){
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        printf("Time is:%ld ",currentTime.tv_sec * (int)1e6 + currentTime.tv_usec);
        return;
}