//
// Created by fudalinm on 13.05.18.
//
#include "header.h"

int counter = 0;
sigset_t set;

int fifoID;
fifo_t* FIFO;
sem_t* BarberSEM;
sem_t* FifoSEM;
sem_t* CheckSEM;


void handler(int sig){
    // printf("dostalem jakis sygnal xDDDDD\n");
    if(sig == SIGRTMAX){
        printf("KLIENT: (%d) wlasnie mnie ostrzygl\n",getpid());
        counter++;
    }
}


void doKLIENT(int LS){
    ///POSIX
    while(counter<LS){
        int tmp = -2;

        if(sem_wait(CheckSEM) == -1 ){printf("KLINET: Nie udalo sie zablokowac semafora CHeck\n"); exit(0);}
        if(sem_wait(FifoSEM) == -1 ){printf("KLIENT: Nie udalo sie zablokowac semafora fifo\n"); exit(0);}

        int barber;
        if(sem_getvalue(BarberSEM,&barber) == -1){printf("nie udalo sie sprawdzic stanu barbera\n"); exit(0);}

        if(barber == 0){

            if(sem_post(BarberSEM) == -1){printf("KLIENT: Nie udalo sie obudzic Barbera\n"); exit(0);}
            printMicro(); printf("KLIENT: (%d) budze barbera\n",getpid());

            FIFO->chair = getpid();

            //po tym tmp == -2
        }else{//brak spiocha
            tmp = pushFIFO(FIFO,getpid());
            //po tym tmp =-1 lub 1
        }

        if(sem_post(FifoSEM) == -1 ){printf("KLIENT: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}
        if(sem_post(CheckSEM) == -1 ){printf("KLIENT: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}

        if(tmp == -1){//nie udalo nam sie siasc
            printMicro();
            printf("KLIENT: (%d) nie dałem rady zajac miejsca w koljce\n",getpid());
            continue;
        }

        if(tmp!= -2){
            printMicro();
            printf("KLIENT: (%d) zajalem miejsce w kolejce czekam az mnie ostrzyga\n",getpid());
            while(1){
                int flag = -1;//zakladamy ze nas nie ma
                if(sem_wait(FifoSEM) == -1 ){printf("KLIENT: Nie udalo sie zablokowac semafora fifo\n"); exit(0);}
                flag = findInQueue(FIFO,getpid());
                if(sem_post(FifoSEM) == -1 ){printf("KLIENT: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}

                if(flag == -1){
                    counter++;
                    printMicro();
                    printf("KLIENT: (%d) wlasnie mnie ostrzygl\n",getpid());
                    break;
                }
            }
        }

    }
    exit(0);

    ///END POSIx

}

void prepareRESOURCES(){
    ///POSIX
    //semy
    BarberSEM = sem_open(bPATH,O_EXCL);
    FifoSEM = sem_open(fPATH,O_EXCL);
    CheckSEM = sem_open(cPATH,O_EXCL);

    if (BarberSEM == SEM_FAILED){printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora1\n"); exit(0);}
    if (FifoSEM == SEM_FAILED)  {printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora2\n"); exit(0);}
    if (CheckSEM == SEM_FAILED) {printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora3\n"); exit(0);}

   //kolejka

    fifoID = shm_open(sPATH, O_RDWR, 0666 );
    if(fifoID == -1) {printf("KLIENT: Nie udalo się stworzyc kolejki wspoldzielonej\n"); exit(0);}

    void* tmp = mmap(NULL,sizeof(fifo_t),PROT_READ | PROT_WRITE | PROT_EXEC , MAP_SHARED , fifoID , 0);
    if(tmp == ((void*) -1) ) {printf("KLIENT: zle zwrocony identyfikator pamieci wspolnej\n"); exit(0);}
    FIFO = tmp;
    ///END POSIX
}

void createCLIENTS(int LS, int LK){
    int i;
    for(i=0;i<LK;i++){
        if (fork() == 0){
            doKLIENT(LS);
            break;
        }else{
            continue;
        }
    }
}

void handlerE(int sig){
    exit(0);
}

void exitFUN(void){
    if(-1 == munmap(FIFO,sizeof(FIFO)) ) {printf("KLIENT: nie udalo sie uwolnic pamieci\n");}

    if (sem_close(BarberSEM) == -1) {printf("KLIENT: nie udalo sie usunac semaforow1\n");}
    if (sem_close(FifoSEM) == -1) {printf("KLIENT: nie udalo sie usunac semaforow2\n");}
    if (sem_close(CheckSEM) == -1) {printf("KLIENT: nie udalo sie usunac semaforow3\n");}
}

int main(int argc, char* argv[]){


    if(argc != 3){
        printf("KLIENT: Zła liczba argumentów\n");
        return 1;
    }

    atexit(exitFUN);
    sigfillset(&set);
    int LK = atoi(argv[1]); //liczba klientow
    int LS = atoi(argv[2]);//liczba strzyzen
    //przygotowanie rzeczy dla klientów
    signal(SIGRTMAX,handler);
    signal(SIGINT,handlerE);
    prepareRESOURCES();

    createCLIENTS(LS,LK);
    wait(NULL);
}
