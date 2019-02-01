//
// Created by fudalinm on 13.05.18.
//
#include "header.h"

key_t key;
int fifoID;
fifo_t* FIFO;
int semID;

void handler(int sig){
    printf("GOLIBRODA: Otrzymalem sygnal SIGTERM\n");
    exit(2);
}

void exitFUN(void) {
    printMicro(); printf("GOLIBRODA: zwalniam zasoby\n");
    //trzeba zwolnic wszystkie semy itd...
    if(shmdt(FIFO) == -1){printf("GOLIBRODA: nie udalo sie uwolnic pamieci\n");}               //usuniecie olaczenia do przestrzeni adresowej
    if(shmctl(fifoID,IPC_RMID,NULL) == -1){printf("GOLIBRODA: nie udalo sie zniszczyc kolejki\n");} //usuwanie kolejki
    if(semctl(semID,0,IPC_RMID) == -1){printf("GOLIBRODA: nie udalo sie usunac semaforow\n");} //usuwanie semaforow zakladam 2 na razie

}

int createFIFO(int IK){
    key = ftok( getenv("HOME") , PROJ);
    if( key == -1 ) {printf("GOLIBRODA: Nie udało się utworzyć klucza(createFIFO)\n"); exit(0);}

    fifoID = shmget(key, sizeof(fifo_t), IPC_CREAT | IPC_EXCL | 0666);
    if(fifoID == -1) {printf("GOLIBRODA: Nie udalo się stworzyc kolejki wspoldzielonej\n"); exit(0);}

    void* tmp = shmat(fifoID,NULL,0);
    if(tmp == ((void*) -1) ) {printf("GOLIBRODA: zle zwrocony identyfikator pamieci wspolnej\n"); exit(0);}

    FIFO = tmp;
    initFIFO(IK,FIFO);
    return 0;
}

int createSEMS(){
    //zakladamy semafor dostemu do FIFO, statystyki barbera,
    semID = semget(key , 3 ,0666 | IPC_CREAT | IPC_EXCL); //czy moze to byc ten sam klucz
    if (semID == -1){printf("GOLIBRODA: nie udalo sie stworzyc semaforow\n");exit(0);}
    if (semctl(semID,BarberSEM,SETVAL,0) == -1) {printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora1\n"); exit(0);} //barber startuje od 0
    if (semctl(semID,FifoSEM,SETVAL,1) == -1) {printf("GOLIBRODA: nie udalo sie ustawic wartosci semafora2\n"); exit(0);} //
    if (semctl(semID,CheckSEM,SETVAL,1) == -1) {printf("GOLIBRODA:nie udalo sie ustawic wartosci semafora3\n"); exit(0);} //moze sprawdzac max jeden na raz trzeba to kontrolowac!!
    return 0;
}

void doGOLIBRODA(){

    while(1){
        pid_t toCut;

        struct sembuf sop;
        sop.sem_flg = 0;

        //ustawiamy semafor barbera
        sop.sem_num = BarberSEM;
        sop.sem_op = -1;

        if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie zablokowac semafora barbera\n"); exit(0);} //to jest miejsce w ktorym spimy

        //zostalismy obudzeni

        //blokujemy FIFO
        sop.sem_num = FifoSEM;
        sop.sem_op = -1;
        if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie zablokowac semafora fifo\n"); exit(0);}

        toCut = FIFO->chair;
        printMicro(); printf("Golibroda: bede strzygł %d\n",toCut);
        FIFO -> chair = 0;

        // kill(toCut,SIGRTMAX);
        //zwalniamy kolejke
        sop.sem_op = 1;
        if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}



        printMicro(); printf("Golibroda: ostrzygłem %d\n",toCut);

//przerabianie calej kolejki
        while(1){

            //blokujemy FIFO
            sop.sem_num = FifoSEM;
            sop.sem_op = -1;
            if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie zablokowac semafora fifo\n"); exit(0);}
            toCut = popFIFO(FIFO);

            if(toCut != -1){//to mamy co strzyc

                //zwalniamy kolejke
                sop.sem_op = 1;
                if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie zwolnic semafora fifo\n"); exit(0);}

                printMicro(); printf("Golibroda: bede strzygł %d\n",toCut);

                //kill(toCut,SIGRTMAX);

                printMicro(); printf("Golibroda: ostrzygłem %d\n",toCut);


            } else{//idziemy spac

                printMicro(); printf("GOLIBRODA: Ide spac!!\n");
                sop.sem_num = BarberSEM;
                sop.sem_op = -1;
                if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie odblokowac semafora barbera\n"); exit(0);} //zmniejszamy barbera o 1

                //zwalniamy kolejke
                sop.sem_num = FifoSEM;
                sop.sem_op = 1;
                if(semop(semID,&sop,1) == -1 ){printf("GOLIBRODA: Nie udalo sie odblokowac semafora fifo\n"); exit(0);}
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
    if( createSEMS() == -1) {printf("GOLIBRODA: nie udalo sie stworzyc semaforów\n");}
    doGOLIBRODA();
}