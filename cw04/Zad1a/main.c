#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

int running = 1;
int wait = 0;

void react_CtrlC(int signal){
   if (wait == 0){
        wait = 1;
   }else {
        wait = 0;
   }
}

void react_CtrlZ(int signal){
    running = 0;
}


int main()
{

            ///ustawiamy reagowanie na sygnaly

    signal(2,react_CtrlZ);      ///Ctr + z pauzujemy

    //ustawienie za pomoca sigaction a nie signal reakcja na koniec programu
    struct sigaction psa;    ///Ctr + c zakonczenie
    psa.sa_handler = react_CtrlC;
    sigaction(20, &psa, NULL);
    //alternatywnie signal(20,react_CtrlC);

    ///ustawiamy daty
     time_t rawtime;
     struct tm * timeinfo;

    while(running){
            sleep(1);           //spimy 2 sekundy
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            printf ( "Nasz czas: %s", asctime (timeinfo) );

        while (wait){
            sleep(1);
        }


    }

    printf("I'm done with you");
    return 0;
}
