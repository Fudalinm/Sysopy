#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int L;
int Type;
int run = 1;

pid_t PPID;
pid_t child = -1;

int UserDeclaredSignal1;
int UserDeclaredSignal2;

int signalsReceivedFromParent = 0;
void ChildReact1(int signal){
    signalsReceivedFromParent++;
    if (signal == SIGUSR1){
        kill(PPID,SIGUSR1);
    } else if (signal == SIGUSR2){
        run = 0;
    }
}

int signalsReceivedFromChild = 0;
void ParentReact1(int signal){

   signalsReceivedFromChild++;
    if (signal == SIGUSR1){
        printf("otrzymalem %d sygnał USR1 od dziecka\n",signalsReceivedFromChild);
    }
}

void ChildReact3(int signal){
    signalsReceivedFromParent++;
    if (signal == UserDeclaredSignal1){
        kill(PPID,UserDeclaredSignal1);
    } else if (signal == UserDeclaredSignal2){
        run = 0;
    }
}

void ParentReact3(int signal){
   signalsReceivedFromChild++;
    if (signal == UserDeclaredSignal1){
        printf("otrzymalem %d sygnał %d od dziecka\n",signalsReceivedFromChild,signal);
    }
}

void CaseFunction1(){

    child = fork();
    if (child != 0){
        signal(SIGUSR1,ParentReact1);
        signal(SIGUSR2,ParentReact1);
        sleep(0.5);

        int signalsSent = 0;
        int i;
        for (i=0;i<L;i++){
            kill(child,SIGUSR1);
            signalsSent++;
        }
        kill(child,SIGUSR2);
        signalsSent++;
        printf("\nWYSLANO SYGNALOW: %d",signalsSent);
        printf("\nOd dziecka odebrano %d",signalsReceivedFromChild);
        printf("\nPrzynajmniej 1 mniej bo nie odeslal SIGUSR2");
    }else{
        signal(SIGUSR1,ChildReact1);
        signal(SIGUSR2,ChildReact1);
        while (run){
            pause();
        }
        printf("\nkoncze swoja prace :)))!\n");
        exit(0);
    }

}

void CaseFunction2(){

    child = fork();

    if (child != 0){
        signal(SIGUSR1,ParentReact1);
        signal(SIGUSR2,ParentReact1);
        sleep(0.5);

        int signalsSent = 0;

        int i;
        for (i=0;i<L;i++){
            kill(child,SIGUSR1);
            signalsSent++;
            pause();
        }
        kill(child,SIGUSR2);
        signalsSent++;
        printf("\nWYSLANO SYGNALOW: %d",signalsSent);
        printf("\nOd dziecka odebrano %d",signalsReceivedFromChild);
        printf("\nPrzynajmniej 1 mniej bo nie odeslal SIGUSR2");
    }else{
        signal(SIGUSR1,ChildReact1);
        signal(SIGUSR2,ChildReact1);
        while (run){
            pause();
        }
        printf("\nkoncze swoja prace :)))!\n");
        exit(0);
    }
}

void CaseFunction3(){

    child = fork();

    if (child != 0){

        //wypada sprawdzic czy udalo sie ustawic
        signal(UserDeclaredSignal1,ParentReact3);
        signal(UserDeclaredSignal2,ParentReact3);
        sleep(0.5);

        int signalsSent = 0;

        int i;
        for (i=0;i<L;i++){
            kill(child,UserDeclaredSignal1);
            signalsSent++;
        }
        kill(child,UserDeclaredSignal2);
        signalsSent++;
        printf("\nWYSLANO SYGNALOW: %d",signalsSent);
        printf("\nOd dziecka odebrano %d",signalsReceivedFromChild);
        printf("\nPrzynajmniej 1 mniej bo nie odeslal SIGUSR2");
    }else{
        signal(UserDeclaredSignal1,ChildReact3);
        signal(UserDeclaredSignal2,ChildReact3);
        while (run){
            pause();
        }
        printf("\nkoncze swoja prace :)))!\n");
        exit(0);
    }
}


void LetTheGamesBegin(){
    PPID = getpid();

    switch (Type){
        case 1:
            CaseFunction1();
        break;
        case 2:
            CaseFunction2();
        break;
        case 3:
            CaseFunction3();
        break;
        default:
            printf("wrong data");
        break;
    }
    return;
}

void endItALL (int signal){
    if(child != -1){
        kill(SIGTERM,child);
    }
    printf("\nOtrzymalem interupta zmywam sie");
}


int main(int argc,char *argv[])
{
    if (argc == 3){
        L = atoi(argv[1]);
        Type = atoi(argv[2]);
    }else if (argc == 5){
        L = atoi(argv[1]);
        Type = atoi(argv[2]);
        UserDeclaredSignal1 = atoi(argv[3]);
        UserDeclaredSignal2 = atoi(argv[4]);

    }else{
        L = 20; //liczba prcesow potomnch
        Type = 3;  //liczba prcesowpo ktorych zezwalamy na dzialanie
        UserDeclaredSignal1 = SIGRTMAX;
        UserDeclaredSignal2 = SIGRTMIN;
    }

    if (Type == 3){ //jesli sa zadeklarowane
        if (UserDeclaredSignal1 < SIGRTMIN || UserDeclaredSignal1 > SIGRTMAX ){
            printf("\nzle wybrany sygnal 1");
            return;
        }
        if (UserDeclaredSignal2 < SIGRTMIN || UserDeclaredSignal2 > SIGRTMAX ){
            printf("\nzle wybrany sygnal 2");
            return;
        }
        if (UserDeclaredSignal2 == UserDeclaredSignal1){
            printf("\nsygnal 1 i 2 nie moga byc tozsame");
            return;
        }
    }

    signal(SIGINT,endItALL);
    LetTheGamesBegin();

}
