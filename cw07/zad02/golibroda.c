//
// Created by fudalinm on 13.05.18.
//
#include "header.h"

int fifoID;
fifo_t* FIFO;
sem_t* BarberSEM;
sem_t* FifoSEM;
sem_t* CheckSEM;

void handler(int sig){
    printf("GOLIBRODA: Otrzymalem sygnal SIGTERM\n");
    exit(2);
}

void exitFUN(void) {
     printMicro(); printf("GOLIBRODA: zwalniam zasoby\n");
//POSIX
    if(-1 == munmap(FIFO,sizeof(FIFO)) ) {printf("GOLIBRODA: nie udalo sie uwolnic pamieci\n");}
    if( shm_unlink(sPATH) == -1) {printf("GOLIBRODA: nie udalo sie zniszczyc kolejki\n");}

    if (sem_close(BarberSEM) == -1) {printf("GOLIBRODA: nie udalo sie usunac semaforow1\n");}
    if(sem_unlink(bPATH) == -1) {printf("GOLIBRODA: nie udalo sie usunac semaforow1\n");}
    if (sem_close(FifoSEM) == -1) {printf("GOLIBRODA: nie udalo sie usunac semaforow2\n");}
    if(sem_unlink(fPATH) == -1) {printf("GOLIBRODA: nie udalo sie usunac semaforow2\n");}
    if (sem_close(CheckSEM) == -1) {printf("GOLIBRODA: nie udalo sie usunac semaforow3\n");}
    if(sem_unlink(cPATH) == -1) {printf("GOLIBRODA: nie udalo sie usunac semaforow3\n");}
}

int createFIFO(int IK){
    //POSIX
    fifoID = shm_open(sPATH , O_CREAT | O_EXCL | O_RDWR , 0666); 
    if(fifoID == -1) {printf("GOLIBRODA: Nie udalo się stworzyc kolejki wspoldzielonej\n"); exit(0);}

    if (ftruncate(fifoID,sizeof(fifo_t)) == -1) {printf("GOLIBRODA: Nie udalo się stworzyc kolejki wspoldzielonej\n"); exit(0);}

    void* tmp = mmap(NULL,sizeof(fifo_t),PROT_READ | PROT_WRITE | PROT_EXEC , MAP_SHARED , fifoID , 0);
    if(tmp == ((void*) -1) ) {printf("GOLIBRODA: zle zwrocony identyfikator pamieci wspolnej\n"); exit(0);}
    FIFO = tmp;
    initFIFO(IK,FIFO);
    return 0;
}

int createSEMS(){

//POSIX STWORZONE
    BarberSEM   = sem_open(bPATH,O_CREAT|O_EXCL,0666,0);
    FifoSEM     = sem_open(fPATH,O_CREAT|O_EXCL,0666,1);
    CheckSEM    = sem_open(cPATH,O_CREAT|O_EXCL,0666,1);

    if (BarberSEM == SEM_FAILED){printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora1\n"); exit(0);}
    if (FifoSEM == SEM_FAILED)  {printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora2\n"); exit(0);}
    if (CheckSEM == SEM_FAILED) {printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora3\n"); exit(0);}
    return 0;

}

void doGOLIBRODA(){


    while(1){
        ///POSIX
        pid_t toCut;

        if(sem_wait(BarberSEM) == -1){printf("GOLIBRODA: Nie udalo sie zablokowac semafora barbera\n"); exit(0);}
        if(sem_post(BarberSEM) == -1){printf("GOLIBRODA: Nie udalo sie obudzic\n"); exit(0);}

        if(sem_wait(FifoSEM) == -1 ){printf("GOLIBRODA: Nie udalo sie zablokowac semafora fifo\n"); exit(0);}

        toCut = FIFO->chair;
        printMicro(); printf("Golibroda: bede strzygł %d\n",toCut);
        FIFO -> chair = 0;

        if(sem_post(FifoSEM) == -1 ){printf("GOLIBRODA: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}

        printMicro(); printf("Golibroda: ostrzygłem %d\n",toCut);

//przerabianie calej kolejki
        while(1){
            if(sem_wait(FifoSEM) == -1 ){printf("GOLIBRODA: Nie udalo sie zablokowac semafora fifo\n"); exit(0);}

            toCut = popFIFO(FIFO);

            if(toCut != -1){

                if(sem_post(FifoSEM) == -1 ){printf("GOLIBRODA: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}
                printMicro(); printf("Golibroda: bede strzygł %d\n",toCut);

                printMicro(); printf("Golibroda: ostrzygłem %d\n",toCut);

            }else{//tu spimy

                printMicro(); printf("GOLIBRODA: Ide spac!!\n");

                if(sem_wait(BarberSEM) == -1) {printf("GOLIBRODA: Nie udalo sie zablokowac semafora barbera\n"); exit(0);}
                if(sem_post(FifoSEM) == -1 ){printf("GOLIBRODA: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}

                break;
            }


        }
    }
}


int main(int argc, char* argv[]){
    if(argc!= 2){printf("GOLIBRODA: Zla liczba argumentów\n"); return 0;}
    int IK = atoi(argv[1]);
    atexit(exitFUN);
    signal(SIGTERM,handler);
    signal(SIGINT,handler);
    if( createFIFO(IK) == -1) {printf("GOLIBRODA: nieudalo sie stworzyc kolejki\n");}
printf("xDDD\n");
    if( createSEMS() == -1) {printf("GOLIBRODA: nie udalo sie stworzyc semaforów\n");}
    doGOLIBRODA();
}
