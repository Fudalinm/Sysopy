#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/// otrzymano efekt zakonczenia ???
/// udzielona zgoda     1
/// nie wyslana prosba -1
/// wyslana prosba      0
#define approval 1
#define sent     0
#define not_sent -1

struct progInf{
    pid_t childs;  ///array of chidren
    int state;     ///array f signals from children
};

volatile int N;          ///processes to start
volatile int M;          ///processes to wait
volatile struct progInf* progStruct;

volatile pid_t PPID;     ///mother PID

volatile int request_received = 0;
volatile int request_sent = 0;

void ParentReact (int signal, siginfo_t *info, void *context){
    if (signal == SIGUSR1){ //jesli napisalo do nas dziecko to zliaczmy ile bylo prosb
        request_received ++;

        pid_t tmp_pid = info->si_pid;

        int i;
        for(i=0;i<N;i++){
            if(progStruct[i].childs == tmp_pid && progStruct[i].state == not_sent){
            progStruct[i].state = sent;
            break;
            }
        }
        //printf("otrzymano prosbe numer: %d\n",request_received);
        if (request_received >= M){//jesli dostalismy ich M lub wiecej to puszczamy wszystkie ktore wyslaly prosbe i nie zostaly jeszcze puszczone
            for(i=0;i<N;i++){
                    if (progStruct[i].state == sent){
                        kill(progStruct[i].childs,SIGCONT);
                        request_sent++;
                        printf("wyslano pozwolenie numer: %d     do procesu: %ld\n",request_sent, (long) progStruct[i].childs);
                        progStruct[i].state = approval;
                    }
            }
        }
    }
    return;
}

int CanGoFurther = 0;
void childReaction(int signal){
    CanGoFurther = 1;
    int signalToSend = SIGRTMIN+rand()%32;
    printf("\nsygnal ktory wysylam:%d\n",signalToSend);
    kill(PPID,signalToSend);
    return;
}

void childFunction(int i){

    signal(SIGCONT,childReaction);
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    /* using nano-seconds instead of seconds */
    srand((time_t)ts.tv_nsec);


    int seconds = rand()%11;
   // printf("\n\nI'm going to sleep for %d\n\n",seconds);
    sleep(seconds);
    kill(PPID,SIGUSR1);
    progStruct[i].state = sent;

    while(!CanGoFurther){
        pause();
    }

    exit(seconds);
}

void end_all(int signal, siginfo_t *info, void *context){

    printf("\nThe time has come\n");
    int i;
    for(i=0;i<N;i++){
        kill( progStruct[i].childs ,SIGTERM );
    }
    if(signal == SIGINT){
        printf("I'm done with you");
    }

    for(i=0;i<N;i++){
        printf("\nNasza tabelka po wykrzaczeniu się | %ld | %d | ",(long) progStruct[i].childs,progStruct[i].state);
    }

    exit(0);
}


int signal_counter = 0;

void printINF (int signal, siginfo_t *info, void *context){
    signal_counter++;
    printf("Dostałem sygnał: %d   %d raz od: %d\n",signal,signal_counter,info->si_pid);
}

void startChildren(){

    PPID = getpid();

    ///setting ALL signals
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    act.sa_sigaction = ParentReact;
    sigaction(SIGUSR1,&act,NULL);

    int i;

    for(i=SIGRTMIN;i<SIGRTMAX+1;i++){
        act.sa_sigaction = printINF;
        sigaction(i,&act,NULL);
    }

    progStruct  =  malloc(N * sizeof(struct progInf) );

    for(i=0;i<N;i++){
        progStruct[i].state  = -1;
        progStruct[i].childs = -1;
    }

    for(i=0;i<N;i++){
        printf("tworze proces numer: %d\n",i);
         progStruct[i].childs = fork();
        if (progStruct[i].childs == 0){ //wtedy jestesmy dzieckiem wiec wychodzimy z tworzenia prcesow
            childFunction(i);
        }
    }

    act.sa_sigaction = end_all;
    sigaction(SIGINT,&act,NULL);


    while(1){
        sleep(00.1);    //czekamy na otrzymanie jakiegos sygnalu
        if (signal_counter == N){
            printf("Koncze program normalnie otrzymalem wszystkie sygnaly");
            return;
        }
    }
}


int main(int argc,char *argv[])
{
    if (argc == 0){
        N = atoi(argv[1]);
        M = atoi(argv[2]);
    }else{
        N = 100; //liczba prcesow potomnch
        M = 50;  //liczba prcesowpo ktorych zezwalamy na dzialanie
    }

   // signal(SIGINT,end_all);
    startChildren();
    return 0;
}
