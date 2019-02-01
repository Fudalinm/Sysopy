#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/resource.h>
#include <inttypes.h>

#define true 0
#define false -1

static const int maxLineLength = 512;
static const int maxArrguments = 32;

int main(int argc,char* argv[])
{
         char* path;
         struct rlimit dMLimit;
         struct rlimit dCLimint;

       if(argc == 4) {
            path = argv[1];
            dMLimit.rlim_max= (rlim_t) (atoi(argv[2])*1024); dMLimit.rlim_cur= (rlim_t) (atoi(argv[2])*1024);
            dCLimint.rlim_max = (rlim_t) (atoi(argv[3]));    dCLimint.rlim_cur = (rlim_t) (atoi(argv[3]));
        }else{
            //jesli nie ma dobrych argumentow to odpalamy testy
            path="/home/fudalinm/Desktop/FudalinskiMarek2/cw03/zad03a/rak1";
            dMLimit.rlim_max= (rlim_t) (1024*210000);dMLimit.rlim_cur= (rlim_t) (1024*200000); //dlaczego potrzebujemy AZ TAK WYSOKICH OGANICZEN
            dCLimint.rlim_max= (rlim_t) (5000000);dCLimint.rlim_cur= (rlim_t) (5000000);

        }
        ///otwarcie pliku
        FILE* toInterprete = fopen(path,"r");
        if (toInterprete == NULL ) {printf("\nblad otwarcia pliku\n"); return false;}
        ///mamy otwarty pliczek

        char* line;
        line = malloc(sizeof(char)*(maxLineLength+2)); ///get line doklada jeden znak w razie czego ('\n')
        pid_t pid;
        int lineCounter = -1;
        char* arguments[maxArrguments+1];

        while(fgets(line,maxLineLength,toInterprete)){ ///przelatujemy kazda z lini naszego pliku wsadowego
                lineCounter ++;

                int k;
                for (k=0;k<maxArrguments+1;k++) {arguments[k]=NULL;}

                ///parujemy pobrana linie
               int i=0;
               int argCounter = -1;

///sprawdzenie pustej linii
                while(line[i]==' '){
                        line[i] = '\0';
                        i++;
                    }
                if(line[i]=='\n'){continue;}
///sprawdzenie pustej linii

                while(line[i] != '\n'){

                    while(line[i]==' '){
                        line[i] = '\0';
                        i++;
                    }
                    if(line[i]!= '\n') {argCounter++;}
                    if(argCounter>maxArrguments) {return false;}
                    if(line[i]!= '\n') {arguments[argCounter] = &line[i];}
                    i++;

                    while(line[i]!=' ' && line[i] !='\n'){
                        i++;
                    }
                    if(line[i] == '\n') {line[i] = '\0'; break;}
                }

                pid = fork();
                if(pid==0){

 ///tutaj mamy nalozyc ograniczenia
 ///!!!!!!!!!!nakladamy TWARDE!!!!!!!!!!!

                    struct rlimit memLimit;
                    struct rlimit coreLimit;

                    getrlimit(RLIMIT_AS,&memLimit);
                    getrlimit(RLIMIT_CPU,&coreLimit);

                    //wypisywanko dziala
                    printf("\nPrzed zmiana ograniczenie na linie %d:\n\n",lineCounter);
                    printf("soft pamieci: %ju \nhard pamieci: %ju\n",(uintmax_t) memLimit.rlim_cur,(uintmax_t)memLimit.rlim_max);
                    printf("soft CPU: %ju \nhard CPU: %ju\n\n",(uintmax_t) coreLimit.rlim_cur,(uintmax_t) coreLimit.rlim_max);

                    setrlimit(RLIMIT_AS,&dMLimit);// jest jakis dziwny problem z ustawieniem limitu pamieci...
                    setrlimit(RLIMIT_CPU,&dCLimint);

                    getrlimit(RLIMIT_AS,&memLimit);
                    getrlimit(RLIMIT_CPU,&coreLimit);

                    printf("\nPo zmianie ograniczenie na linie %d:\n\n",lineCounter);
                    printf("soft pamieci: %ju \nhard pamieci: %ju\n",(uintmax_t) memLimit.rlim_cur,(uintmax_t)memLimit.rlim_max);
                    printf("soft CPU: %ju \nhard CPU: %ju\n\n",(uintmax_t) coreLimit.rlim_cur,(uintmax_t) coreLimit.rlim_max);

///KONIEC NAKLADANIA OGRANICZEN

                    if(*(arguments[0]) == '.' || *(arguments[0]) == '/' ){
                        execv(arguments[0], arguments );
                    }else{
                        execvp(arguments[0],arguments );
                    }
                    exit(-1);
                }
                if(pid!=0){

                    int status = 0;
                    wait(&status);

                    if (status != 0){ //po zanalezeniu jednego bledu lcimy dalej
                        printf("\nZlapalismy errora w linii (liczac od 0) %d\n", lineCounter);

                    }else{
                        //int getrusage(int who, struct rusage *usage);
                        struct rusage raport;
                        if(getrusage(RUSAGE_CHILDREN,&raport) == 0){
                            //mamy raportowac czas uzytkownika i rzeczywisty
                            printf("\nPROCES MATKA ZGLASZA RAPORT DO LINI %d:\n",lineCounter);
                            printf("   -czas użytkownika:%ld.%06ld\n",raport.ru_utime.tv_sec,raport.ru_utime.tv_usec);
                            printf("   -czas rzeczywisty:%ld.%06ld\n\n",raport.ru_stime.tv_sec,raport.ru_stime.tv_usec);

                        }else{
                            printf("\n\n!MACIERZYSTY!\nnie udalo nam sie sprawdziczuzycia !\n\n");
                        }
                    }
                }
        }
        fclose(toInterprete);
        return true;
}
