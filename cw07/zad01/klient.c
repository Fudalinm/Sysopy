//
// Created by fudalinm on 13.05.18.
//
#include "header.h"

key_t key;
int fifoID;
fifo_t* FIFO;
int semID;
int counter = 0;
sigset_t set;

void handler(int sig){
    // printf("dostalem jakis sygnal xDDDDD\n");
    if(sig == SIGRTMAX){
        printf("KLIENT: (%d) wlasnie mnie ostrzygl\n",getpid());
        counter++;
    }
}


void doKLIENT(int LS){


    struct sembuf sop;
    sop.sem_flg = 0;

    while(counter < LS){
        int tmp = -2;

        //BIERZEMY CHECERA
        sop.sem_num = CheckSEM;
        sop.sem_op = -1;
        if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie zablokowac SEMCHECK\n"); exit(0);}

        //BIERZEMY FIFO
        sop.sem_num = FifoSEM;
        sop.sem_op = -1;
        if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie zablokowac Fifo\n"); exit(0);}

        //trzeba sprawdzic co robi barber
        int barber = semctl(semID,BarberSEM,GETVAL);
        if(barber == -1){printf("nie udalo sie sprawdzic stanu barbera\n"); exit(0);}

        if(barber == 0){//to mamy spiocha

            sop.sem_num = BarberSEM;
            sop.sem_op = 1;
            if (semop(semID,&sop,1) == -1){printf("KLIENT: Nie udalo sie obudzic Barbera\n"); exit(0);}
            printMicro(); printf("KLIENT: (%d) budze barbera\n",getpid());
            if (semop(semID,&sop,1) == -1){printf("KLIENT: Nie udalo sie obudzic Barbera\n"); exit(0);}

            FIFO->chair = getpid();

            //po tym tmp = -2
        }else{//nie mamy spiocha wiec PROBUJEMY siasc w kolejce
            tmp = pushFIFO(FIFO,getpid());
            //po tym tmp 1 lub -1
        }
        //oddajemy zasoby
        sop.sem_num = FifoSEM;
        sop.sem_op = 1;
        if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie odblokowac Fifo\n"); exit(0);}

        sop.sem_num = CheckSEM;
        sop.sem_op = 1;
        if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie odblokowac SEMCHECK\n"); exit(0);}

        if(tmp == -1){//nie udalo nam sie siasc
            printMicro();
            printf("KLIENT: (%d) nie dałem rady zajac miejsca w koljce\n",getpid());
            continue;
        }

        //tu czekamy az nas ostrzyze
        if(tmp != -2){
            printMicro();
            printf("KLIENT: (%d) zajalem miejsce w kolejce czekam az mnie ostrzyga\n",getpid());
            while(1){
                int flag = -1;//zakladamy ze nas nie ma

                //BIERZEMY FIFO
                sop.sem_num = FifoSEM;
                sop.sem_op = -1;
                if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie zablokowac Fifo\n"); exit(0);}
/*
                for(int i=0;i<FIFO->max+1;i++){
                    if(FIFO->queue[i] == getpid()){
                        flag = 1;
                        break;
                    }
                }

                if(FIFO->chair == getpid()){
                    flag = 1;
                }
*/
                flag = findInQueue(FIFO,getpid());

                //oddajemy FIFO
                sop.sem_num = FifoSEM;
                sop.sem_op = 1;
                if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie odblokowac Fifo\n"); exit(0);}

                if(flag == -1){
                    counter++;
                    printMicro();
                    printf("KLIENT: (%d) wlasnie mnie ostrzygl\n",getpid());
                    break;
                }
                //sigsuspend(&set);
            }
        }
/*
        if(tmp == -2){
            printf("KLIENT: (%d) powinienem byc na krzesle\n",getpid());
            while(1){
                //BIERZEMY FIFO
                sop.sem_num = FifoSEM;
                sop.sem_op = -1;
                if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie zablokowac Fifo\n"); exit(0);}


                if(FIFO->chair != getpid()){
                    counter++;
                    printf("KLIENT: (%d) wlasnie mnie ostrzygl\n",getpid());
                    break;
                }

                //oddajemy FIFO
                sop.sem_num = FifoSEM;
                sop.sem_op = 1;
                if( semop(semID,&sop,1) == -1 ) {printf("KLIENT: Nie udalo sie odblokowac Fifo\n"); exit(0);}
                // sigsuspend(&set);
            }

            //sigsuspend(&set);
        }
*/
    }
    exit(0);
}

void prepareRESOURCES(){
    key = ftok( getenv("HOME") , PROJ);
    if( key  == -1 ) {printf("KLIENT: Nie udało się utworzyć klucza\n"); exit(0);}

    semID  = semget(key,0,0);
    if(semID == -1){printf("KLIENT: nie udalo sie pobrac id semafora\n"); exit(0);}

    fifoID = shmget(key,0,0);
    if(fifoID == -1){printf("KLIENT: nie udalo sie pobrac id kolejki\n"); exit(0);}

    void* tmp = shmat(fifoID,NULL,0);
    if(tmp ==  ((void*) -1)){ printf("KLIENT: nie udalo sie pobrac wskaznika na kolejke\n"); exit(0);}
    FIFO = tmp;
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
    //printf("KLIENT: zwalniam zasoby\n");
    shmdt(FIFO);  //odlaczenia do przestrzeni adresowej
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

    /*
    sigset_t set2;
    sigemptyset(&set2);
    sigaddset(&set2,SIGRTMAX);
    sigprocmask(SIG_BLOCK,&set2,NULL);
    */

    createCLIENTS(LS,LK);
    wait(NULL);
}