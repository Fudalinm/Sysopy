#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define true 0
#define false -1

static const int maxLineLength = 512;
static const int maxArrguments = 32;

int main(int argc,char* argv[])
{
    ///mam dostac sciezke do pliku do zinterpretowania!
   /* if (argc != 2){
        printf("bledne argumenty\n");
        return false;
    }*/
         char* path;
       if(argc == 2) {path = argv[1];}else{path="/home/fudalinm/Desktop/Zad02/bin/Debug/rak1";} //sciezynka do pliku jest juz zapisana
        ///wypada ja teraz otworzyc ale na wszelki wypadek wezmy bezwzgledna



        FILE* toInterprete = fopen(path,"r");
        if (toInterprete == NULL ) {printf("\nblad otwarcia pliku\n"); return false;}
        ///mamy otwarty pliczek

        ///zakladamy ze polecenia sa pisane tak:
        /// linia1: command1 agr11 arg12 ....
        /// linia2: command2 arg21 arg22 ....
        /// .................................
        ///nalezy wczytac pelna linie do odpalenia

        char* line;
        line = malloc(sizeof(char)*(maxLineLength+2)); ///get line doklada jeden znak w razie czego
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

                    if (status != 0){
                        printf("\nZlapalismy errora w linii (liczac od 0) %d\n", lineCounter);

                    }
                }
        }
        fclose(toInterprete);
        return true;
}
