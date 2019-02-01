#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

pid_t child = -1;
int running = 1;

void funkcjaDziecko(){
        while (child == 0){
                //tutaj nieskonczony skrypt shallowy
                execl("skrypt",NULL);
                sleep(2);
        }
}

void react_CtrlC(int signal){ ///tutaj mamy zabic dziecko jesli jest i zakonczyc siebie,jesli nie ma to zabic tylko siebie
    if(child != 0 ){
        if (child == -1){ ///jesli nie mialem dziecka to je wywoluje
            running = 0; //po prostu konczymy siebie
        }else{
            kill(child,SIGKILL); //zabijamy dziecko
            child = -1;
            running = 0;
        }
    }
}

void react_CtrlZ(int signal){ ///tu rodzic ma zabic dziecko lub stworzyc nowe jesli takowego nie bylo
    if(child != 0 ){
        if (child == -1){ ///jesli nie mialem dziecka to je tworze i robi swojego execa
            child = fork();
            if(child == 0){//wykonaj funkcje dziecka
                funkcjaDziecko();
            }
        }else{///tutaj zabijamy dziecko ktore to podobno bylo
            kill(child,SIGKILL); //zabijamy dziecko
            child = -1;
        }
    }
}


int main()
{
    //wychodzi na ctrlZ

            ///ustawiamy reagowanie na sygnaly

    signal(20,react_CtrlZ);      ///Ctr + z pauzujemy
    //ustawienie za pomoca sigaction a nie signal reakcja na koniec programu
    struct sigaction psa;    ///Ctr + c zakonczenie
    psa.sa_handler = react_CtrlC;
    sigaction(2, &psa, NULL);
    //alternatywnie signal(20,react_CtrlC);

    child = fork();
    if (child == 0){
        funkcjaDziecko();
    }

    while(running){
        sleep(0.5);
    }

    printf("I'm done with you");
    return 0;
}
