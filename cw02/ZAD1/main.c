#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>

//tu sobie zdefiniujemy kazda z funkcji zacznijmy od generowania

int generate(char* filePath, int sizeOfblock, int numberOfBlocks){
    // tworzymy plik o zadanej sciezynce jezeli byl to rip i go usuniemy
    FILE *creation = fopen(filePath,"w+");
    FILE *randFILE = fopen("/dev/urandom","r");

    //teraz wypada stworzyc dane ktore chcemy mu zapisac
    char* randominium = malloc(sizeof(char)*sizeOfblock + 2); //cały blok pamięci

   // size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
   // The function fwrite() writes nmemb elements of data, each size bytes long, to the stream pointed to by stream, obtaining them from the location given by ptr.
    int i;
    for(i=0;i<numberOfBlocks;i++){  //chcemy zapisac numberofblocks blokow

       //potrzebujemy odczytac blok pamieci z randoma
        fread(randominium,sizeof(char),(size_t)(sizeOfblock + 1),randFILE);
        //obrabiamy random zeby zawieralo znaki od 32-126
        int j;
        for(j=0;j<sizeOfblock+1;j++){
            randominium[j] = (char) (randominium[j]%30 + 65);
        }

        fwrite(randominium,sizeof(char),sizeOfblock,creation);
        fprintf(creation, "\n"); //rozdzielamy bloki znakiem konca linii

    }

    //free(randominium);
    fclose(creation);
    fclose(randFILE);
    return 1;
}


//bardzo wazne fwrite NADPISUJE !!!! A NIE DOKLEJA
int libSort (char* inPath,int sizeOfBlock,int numberOfBlocks){ //przy uzyciu fread fwrite
    //wiec tak bierzemy drugi sprawdzamy czy mniejszy od pierwszego jak tak to swap itd
    FILE *toSort = fopen(inPath,"r+"); //otwieramy do zapisu i odczytu
    if (toSort == NULL) return -1;
    //tworzy dwa rekordy
    char* rec1 = malloc(sizeof(char)*sizeOfBlock + 1);
    char* rec2 = malloc(sizeof(char)*sizeOfBlock + 1);

    int i;
    for (i=1;i<numberOfBlocks;i++){
        //musimy zapisac sobie i'ty rekord wiec musimy odpowiednio ustawic nasz kursor
        fseek(toSort,(long) (i)*(sizeOfBlock + 1),SEEK_SET); //ustawiamy wskaznik na poczatek segmentu ktory bedziemy przenosic
        fread(rec1,sizeof(char),(size_t)(sizeOfBlock + 1),toSort);//wczytujemy zadany segment

        int j;
        for(j=0;j<i;j++){//idziemy az do poczatku rekordow
            fseek(toSort,(long) (j)*(sizeOfBlock + 1),SEEK_SET );//ustawiamy na poczatek segmentu do pobrania zerby porownac
            fread(rec2,sizeof(char),(size_t)(sizeOfBlock + 1),toSort); //pobieramy drugi rekord aby porownac
            //jesli pierwszy znak mniejszy to przesuwamy w lewo jesli nie to sprawdzamy dalej
            if ( (unsigned char) rec1[0] < (unsigned char) rec2[0]  ){//jesli tak to przenosimy rec1
                 //trzeba je zamienic a potem przeniec reszte elementow
                 fseek(toSort,(long) (j)*(sizeOfBlock + 1),SEEK_SET);
                 fwrite(rec1,sizeof(char),sizeOfBlock + 1,toSort);
                 j++;
                //zamienilismy oba elemnty teraz trze przeniec wszystko
                 for(;j<i;j++){//przesuwamy elementy
                    fseek(toSort,(long) (j)*(sizeOfBlock + 1),SEEK_SET);
                    fread(rec1,sizeof(char),(size_t)(sizeOfBlock + 1),toSort);
                    fwrite(rec2,sizeof(char),sizeOfBlock + 1,toSort);

                    rec2 = rec1;//podstawimy element ktory byl odczytany i nalezy go dalej przeniesc
                 }
                break;//zrobilismy w tej petli to co bylo trzeba
            }else {continue;} //jesli nie to szukamy dalej
        }
    }

    fclose(toSort);
    //nie wiem czemu ale przez to sie wysypywal
   // free(rec1);
   // free(rec2);

    return 1;
}


int sysSort (char* inPath,int sizeOfBlock,int numberOfBlocks){//przy uzyciu read write
     //wiec tak bierzemy drugi sprawdzamy czy mniejszy od pierwszego jak tak to swap itd
    int toSort = open(inPath, O_RDWR);//otwieramy do zapisu i odczytu
    //tworzy dwa rekordy
    char* rec1 = malloc(sizeof(char)*sizeOfBlock + 1);
    char* rec2 = malloc(sizeof(char)*sizeOfBlock + 1);
    long int offset = (long int) ((sizeOfBlock + 1) * sizeof(char));

    int i;
    for (i=1;i<numberOfBlocks;i++){
        //musimy zapisac sobie i'ty rekord wiec musimy odpowiednio ustawic nasz kursor
        lseek(toSort, i * offset, SEEK_SET);//ustawiamy wskaznik na poczatek segmentu ktory bedziemy przenosi
        read(toSort,rec1,(size_t)(sizeOfBlock + 1)* sizeof(char));//wczytujemy zadany segment

        int j;
        for(j=0;j<i;j++){//idziemy az do poczatku rekordow
            lseek(toSort, j * offset, SEEK_SET);//ustawiamy na poczatek segmentu do pobrania zerby porownac
            read(toSort,rec2,(size_t)(sizeOfBlock + 1)* sizeof(char));//pobieramy drugi rekord aby porownac
            //jesli pierwszy znak mniejszy to przesuwamy w lewo jesli nie to sprawdzamy dalej
            if ( (unsigned char) rec1[0] < (unsigned char) rec2[0]  ){//jesli tak to przenosimy rec1
                 //trzeba je zamienic a potem przeniec reszte elementow
                 lseek(toSort, j * offset, SEEK_SET);
                 write(toSort, rec1, sizeof(char) * (sizeOfBlock + 1));
                 j++;
                //zamienilismy oba elemnty teraz trze przeniec wszystko
                 for(;j<i;j++){//przesuwamy elementy

                    lseek(toSort, j * offset, SEEK_SET);
                    read(toSort,rec1,(size_t)(sizeOfBlock + 1)* sizeof(char));
                    write(toSort, rec2, sizeof(char) * (sizeOfBlock + 1));

                    rec2 = rec1;//podstawimy element ktory byl odczytany i nalezy go dalej przeniesc
                 }
                break;//zrobilismy w tej petli to co bylo trzeba
            }else {continue;} //jesli nie to szukamy dalej
        }

    }
    close(toSort);
   // free(rec1);
   // free(rec2);
    return 1;
}



int sysCopy (char* inPath,char* copyPath,int sizeOfBlock,int numberOfBlocks){//przy uzyciu read write

    int toCopy = open(inPath, O_RDWR);
    int toBeCoppied = open(copyPath, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    char* rec = malloc(sizeof(char)*sizeOfBlock + 1);

    int i;
    for(i=0;i<numberOfBlocks;i++){
        read(toCopy,rec,(size_t)(sizeOfBlock + 1)* sizeof(char));
        write(toBeCoppied,rec, sizeof(char) * (sizeOfBlock + 1));
    }

    return 1;

}



int libCopy (char* inPath,char* copyPath,int sizeOfBlock,int numberOfBlocks){//przy uzyciu fread fwrite

    FILE *toCopy = fopen(inPath,"r");
    if (toCopy == NULL) return -1;

    FILE *tobeCoppied = fopen(copyPath,"w+");
    if (tobeCoppied == NULL) return -1;

    char* rec = malloc(sizeof(char)*sizeOfBlock + 1);


    int i;
    for(i=0;i<numberOfBlocks;i++){
         fread(rec,sizeof(char),(size_t)(sizeOfBlock + 1),toCopy);
         fwrite(rec,sizeof(char),sizeOfBlock + 1,tobeCoppied);
    }

    fclose(toCopy);
    fclose(tobeCoppied);
    return 1;
}



int main(int argc, char *argv[])
{
    /// generate("hue21.txt",200,120);
    ///sysCopy("hue21.txt","kopia.txt",200,120);
    ///generate("hue21.txt",200,120); sysCopy("hue21.txt","kopia.txt",200,120);
    ///generate("hue21.txt",200,120); libSort("hue21.txt",200,120);sysSort("hue21.txt",200,120);
    struct tms procStart;
    struct tms procEnd;

    int64_t userAmp;
    int64_t coreAmp;

   if (argc > 2){
        if((strcmp(argv[1],"s1") == 0 || strcmp(argv[1],"s2") == 0) && argc == 5){


            char* inputFilePath = argv[2];
            int blocks = atoi(argv[3]);
            int sizeOfBlock = atoi(argv[4]);

            if(strcmp(argv[1],"s1") == 0 ){
                times(&procStart);

                sysSort(inputFilePath,sizeOfBlock,blocks);

                times(&procEnd);

                userAmp=procEnd.tms_utime - procStart.tms_utime;
                coreAmp=procEnd.tms_stime - procStart.tms_stime;
                printf("wywołanie: bloków %d, blocksize %d, operations: %s",blocks,sizeOfBlock,argv[1]);
                printf("Czas użytkownika: %ld \n",userAmp);
                printf("Czas procesora: %ld \n\n",coreAmp);



            }else{
                times(&procStart);

                libSort(inputFilePath,sizeOfBlock,blocks);

                times(&procEnd);

                userAmp=procEnd.tms_utime - procStart.tms_utime;
                coreAmp=procEnd.tms_stime - procStart.tms_stime;
                printf("wywołanie: bloków %d, blocksize %d, operations: %s",blocks,sizeOfBlock,argv[1]);
                printf("Czas użytkownika: %ld \n",userAmp);
                printf("Czas procesora: %ld \n\n",coreAmp);

            }



        }else if((strcmp(argv[1],"c1") == 0  || strcmp(argv[1],"c2") == 0 ) && argc == 6){

            char* inputFilePath = argv[2];
            char* outputFilePath = argv[3];
            int blocks =atoi(argv[4]);
            int sizeOfBlock = atoi(argv[5]);

            if(strcmp(argv[1],"c1") == 0 ){

                times(&procStart);

                sysCopy(inputFilePath,outputFilePath,sizeOfBlock,blocks);

                times(&procEnd);

                userAmp=procEnd.tms_utime - procStart.tms_utime;
                coreAmp=procEnd.tms_stime - procStart.tms_stime;
                printf("wywołanie: bloków %d, blocksize %d, operations: %s",blocks,sizeOfBlock,argv[1]);
                printf("Czas użytkownika: %ld \n",userAmp);
                printf("Czas procesora: %ld \n\n",coreAmp);

            }else{

                times(&procStart);

                libCopy(inputFilePath,outputFilePath,sizeOfBlock,blocks);

                times(&procEnd);

                userAmp=procEnd.tms_utime - procStart.tms_utime;
                coreAmp=procEnd.tms_stime - procStart.tms_stime;
                printf("wywołanie: bloków %d, blocksize %d, operations: %s",blocks,sizeOfBlock,argv[1]);
                printf("Czas użytkownika: %ld \n",userAmp);
                printf("Czas procesora: %ld \n\n",coreAmp);

            }


        }else if(strcmp(argv[1],"g") == 0 && argc == 5){

            char* outputFilePath = argv[2];
            int blocks = atoi(argv[3]);
            int sizeOfBlock = atoi(argv[4]);

            generate(outputFilePath,sizeOfBlock,blocks);

        }else {

        return -1;
        }
    }
    return 1;
}
