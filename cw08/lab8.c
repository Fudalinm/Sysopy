//
// Created by fudalinm on 17.05.18.
//
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>

///o macierzy wejsciowej
int* inMatrixPointer;
int wightIN; //zazwyczaj szersze niz wyzsze
int heightIN;
int maxHUEIN; //HUE = odcien

///o macierzy filtru
double* filterMatrixPointer;
int filterSize; //podany jako bok kwadratu!!!

///macierz wyjsciowa
int* outMatrixPointer;//rozmiary identyczne jak wejsciowej

struct args_t{
    int startPixel;
    int endPixel;
} args_t;

int LW = -1;//liczba watkoow
FILE* inFILE;
FILE* filterFILE;
FILE* outFILE;

///do zwracania indeksow macierzy
int max(int a,int b){
    if(a > b) {return a;}
    return b;
}

int min2(int a, int b){//z jakiegos powodu min nie dziala?
    if(a>b) {return b;}
    return a;
}

int row(int pixel,int i){
    int min  = 0; //0 czy 1??? ///0 bo presuniecie indeksowania macierzy...
    int alternative;                                                         ///-1 bo nasz macierz jest przesunieta o 1
    alternative = pixel/(wightIN) - (int) ceil((double) filterSize/2.0 ) + i -1;  //numer wiersza - sufit z rozmiaru/2
    return min2( max(min,alternative) , heightIN - 1 );
}

int column(int pixel, int j){
    int min = 0; //0 czy 1??? //0 bo przesuniecie indeksowania macierzy...
    int alternative;
    alternative = pixel%(wightIN) - (int) ceil((double) filterSize/2.0 ) + j -1; ///-1 zuwagi na to ze nasza macierz jest przesunieta o tye
    return min2(max(min,alternative) , wightIN - 1);
}

///funkcja konczaca
void exitFUN(void){
    if (fclose(inFILE) != 0)     {printf("Nie udalo sie zamknac inFile\n");}
    //if (fclose(filterFILE) != 0) {printf("Nie udalo sie zamknac filterFile\n");} dlaczego zamykanie tego powoduje SEGMENTATION F...
    if (fclose(outFILE) != 0)    {printf("Nie udalo sie zamknac outFile\n");}
}


///funkcja dla watkow
void* filter(void* ptr){
    struct args_t* arguments;
    arguments = ptr;
    //mamy pixele na ktorych mamy wykonac operacje wystarczy teraz ja zrobic xDDDD
    int cStartPixel = arguments->startPixel;
    int cEndPixel = arguments->endPixel;

    while(cStartPixel < cEndPixel){
        double sum = 0;
        int i = 0;
        ///realizujemy nasza sigme
        while(i<filterSize){
            int rowMIn = row(cStartPixel,i); 
            int j = 0;
            while(j<filterSize){
                int colMIn = column(cStartPixel,j); if(rowMIn*wightIN + colMIn > wightIN*heightIN){printf("xDDDD %d\n Wiersz:%d Kolumna:%d\n ",rowMIn*wightIN + colMIn,rowMIn,colMIn);} 
                sum += inMatrixPointer[rowMIn*wightIN + colMIn]*filterMatrixPointer[i*filterSize+j];
                j++;
            }
            i++;
        }
        ///przypisujemy oblicza wartosc do out
        outMatrixPointer[cStartPixel] = (int)round(sum); //printf("indeks: %d wartosc: %d\n",cStartPixel,outMatrixPointer[cStartPixel]);
        cStartPixel++;
    }
	return (void*) 1;
}


int main(int argc,char* argv[]) {

    if(argc != 5){
        printf("zla liczba argumentów\n");
        printf("LICZBA WATKOW | INFILE | FILTERFILE | OUTFILE\n");
        return 0;
    }
    atexit(exitFUN);
    LW = atoi(argv[1]);

    //tworzenie plikow
    inFILE = fopen(argv[2],"r");
    if(inFILE == NULL){printf("Nie udalo sie tworzyc IN'a\n");exit(0);}
    FILE* filterFILE = fopen(argv[3],"r");
    if(filterFILE == NULL){printf("Nie udalo sie otworzyc filterFile\n"); exit(0);}
    outFILE = fopen(argv[4],"a");
    if(outFILE == NULL){printf("Nie udalo otworzyc sie pliku wyjsciowego\n");exit(0);}

    char buf[256];

    //ogarniamy macierz wejsciowa
    char* token;

    fgets(buf,256,inFILE); //jakies P2
    fgets(buf,256,inFILE); //created by...
    fgets(buf,256,inFILE); //szerokosc i wysokosc
    token = strtok(buf," ");
    wightIN = atoi(token);
    token = strtok(NULL," ");
    heightIN = atoi(token);

    fgets(buf,256,inFILE);
    token = strtok(buf," ");
    maxHUEIN = atoi(token);

    int matrixIN[wightIN*heightIN]; //macierz musi byc jedno wymiarowa
    int i =0;
    while (fgets(buf,256,inFILE)){//uzupelniamy nasza macierz
        token = strtok(buf," ");
        while(token){
            matrixIN[i] = atoi(token);
            token = strtok(NULL," ");
            i++;
        }
    }

    ///inicjalizacja macierz filtru

        fgets(buf,256,filterFILE);
        token = strtok(buf," ");
        filterSize = atoi(token);

        double matrixFILTER[filterSize*filterSize]; //macierz o odczytanej wielkosci
        i =0;
		
        while (fgets(buf,256,filterFILE)){//uzupelniamy nasza macierz
            token = strtok(buf," ");
            while(token){
                matrixFILTER[i] = atof(token);
                token = strtok(NULL," ");
                i++;
            }
        }

    ///inicjalizacji macierzy wyjsciowej
        int matrixOUT[wightIN*heightIN];
        for(i=0;i<wightIN*heightIN;i++){
            matrixOUT[i] = 0;
        }
	//zle wczytuje filtr dziwne rzeczy ma zapisane
	

	
    ///zapisanie wskaznikow na wszystkie macierze dla watkow
        inMatrixPointer = matrixIN;
        filterMatrixPointer = matrixFILTER;
        outMatrixPointer = matrixOUT;

    ///ogarnianie argumentów dla nich

        pthread_t threads[LW];
        int skok = (wightIN*heightIN)/LW; //tyle pixeli ma ogarnac kazdy watek
        int pixelCounter = 0;


        struct args_t arguments[LW];
        for(i=0;i<LW-1;i++){
            arguments[i].startPixel = pixelCounter ;
            arguments[i].endPixel = pixelCounter + skok - 1;
            pixelCounter = pixelCounter + skok;
        }

        arguments[LW-1].startPixel = pixelCounter;
        arguments[LW-1].endPixel = heightIN*wightIN;
    ///pobieramy czas
   struct timespec tm1;
   struct timespec tm2;
   clock_gettime(CLOCK_REALTIME,&tm1);

    ///wywolanie watkow
       for(i=0;i<LW;i++){
           pthread_create(&(threads[i]),NULL,filter,&arguments[i]);
       }

    ///oczekiwanie na wykonanie sie watkow
        for(i=0;i<LW;i++){
            pthread_join(threads[i],NULL);
        }
    ///pobieramy czas koncowy
    clock_gettime(CLOCK_REALTIME,&tm2);

    printf("Czas startu: sekundy: %lld  nanosekundy %9ld  \n Czas końca: sekundy: %lld  nanosekundy %9ld   \n  Roznica: sekundy: %lld  nanosekundy %9ld \n\n", (long long) tm1.tv_sec,tm1.tv_nsec, (long long) tm2.tv_sec,tm2.tv_nsec, (long long) tm2.tv_sec-tm1.tv_sec,tm2.tv_nsec - tm1.tv_nsec);

    //TODO:przepisanie otrzymanej macierzy zapisanie do pliku
    //mamy gotowego out'a trzeba go tylko przepisac

     fprintf(outFILE,"P2\n");
     fprintf(outFILE,"#Created by Marek Fudaliński\n");
     fprintf(outFILE,"%d %d\n",wightIN,heightIN);
     fprintf(outFILE,"%d\n",maxHUEIN);
     for(i=0;i<wightIN*heightIN;i++){
         fprintf(outFILE,"%d ",outMatrixPointer[i]);
     }
	
	//segmentation fault dopiero w endingu....
	//printf("\n##Nie lapie segma\n");

    return 0;
}
