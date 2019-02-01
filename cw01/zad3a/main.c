#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include "lib.h"
#define true 1
#define false 0
#define Miliord 1000000000

//returns -1 if error
//returns 1 if ok

//wypisywanie rzykladowych wywołań
//musimy zapisac otrzymane winiki
//robimy to wyołując program w konsoli            main.c ARGS >> raport2.txt

int main(int argc, char *argv[])
{
/*
    printf("%d\n",argc);
    int i1;
    for(i1=0; i1< argc;i1++)
        printf("%d>%s",i1,argv[i1]);
*/


    if(argc != 6) return -1;
    int size = atoi(argv[1]);
    int blockSize = atoi(argv[2]);
    int wayOfAllock = atoi(argv[3]);
    int operation = atoi(argv[4]);
    int n = atoi(argv[5]); //ilosc operacji

    char* randominium= malloc(blockSize);
    int* randominium2 = malloc(blockSize);

    struct timespec realStart;
    struct timespec realEnd;

    struct tms procStart;
    struct tms procEnd;

    double realAmp;
    int64_t userAmp;
    int64_t coreAmp;

    if (wayOfAllock == 1){

        if (operation == 0){//stworzenie tablicy, usunięcie i dodanie zadanej liczby bloków


            //tu trzeba zaczac mierzyc czasy etc
            clock_gettime(CLOCK_REALTIME,&realStart);
            times(&procStart);
            //inicjalizacja tabliczki
            allockTable(size,blockSize);
            int i;

            for (i=0;i<size;i++){
                addElem(i,randominium);
            }

            //dodanie elementow
            for (i=0;i<n;i++){
                addElem(i%size,"ala ma kota");
            }

            //usuniecie elementow

            for(i=0;i<n;i++){
                removeElem(i%size);
            }
            clock_gettime(CLOCK_REALTIME,&realEnd);
            times(&procEnd);

            realAmp=(realEnd.tv_sec - realStart.tv_sec )*Miliord+(realEnd.tv_nsec - realStart.tv_nsec);
            userAmp=procEnd.tms_utime - procStart.tms_utime;
            coreAmp=procEnd.tms_stime - procStart.tms_stime;

            printf("wywołanie: size %d, blocksize %d, wayOfAllock: %d, operations set: %d, numbers of operations %d\n",size,blockSize,wayOfAllock,operation,n);
            printf("Czas rzeczywisty: %lf \n",realAmp);
            printf("Czas użytkownika: %ld \n",userAmp);
            printf("Czas procesora: %ld \n\n",coreAmp);
            return 1;

        } else if (operation == 1) {//stworzenie tablicy, wyszukanie elementu oraz usunięcie i dodanie zadanej liczby bloków


            //tu trzeba zaczac mierzyc czasy etc
            clock_gettime(CLOCK_REALTIME,&realStart);
            times(&procStart);
            //inicjalizacja tabliczki
            allockTable(size,blockSize);
            int i;

            for (i=0;i<size;i++){
                addElem(i,randominium);
            }

            //dodanie elementow
            for (i=0;i<n;i++){
                addElem(i%size,"ala ma kota");
            }

            serchClosest(*randominium2);

            //usuniecie elementow
            for(i=0;i<n;i++){
                removeElem(i%size);
            }
            clock_gettime(CLOCK_REALTIME,&realEnd);
            times(&procEnd);

            realAmp=(realEnd.tv_sec - realStart.tv_sec )*Miliord+(realEnd.tv_nsec - realStart.tv_nsec);
            userAmp=procEnd.tms_utime - procStart.tms_utime;
            coreAmp=procEnd.tms_stime - procStart.tms_stime;

            printf("wywołanie: size %d, blocksize %d, wayOfAllock: %d, operations set: %d, numbers of operations %d\n",size,blockSize,wayOfAllock,operation,n);
            printf("Czas rzeczywisty: %lf \n",realAmp);
            printf("Czas użytkownika: %ld \n",userAmp);
            printf("Czas procesora: %ld \n\n",coreAmp);

            return 1;


        }else if (operation == 2) {//stworzenie tablicy naprzemienne usunięcie i dodanie zadanej liczby bloków


             //tu trzeba zaczac mierzyc czasy etc
            clock_gettime(CLOCK_REALTIME,&realStart);
            times(&procStart);
            //inicjalizacja tabliczki
            allockTable(size,blockSize);
            int i;

            for (i=0;i<size;i++){
                addElem(i,randominium);
            }

            //naprzemienne usunięcie i dodanie bloków
            for (i=0;i<n;i++){
                removeElem(i%size);
                addElem(i%size,"ala ma kota");
            }


            clock_gettime(CLOCK_REALTIME,&realEnd);
            times(&procEnd);

            realAmp=(realEnd.tv_sec - realStart.tv_sec )*Miliord+(realEnd.tv_nsec - realStart.tv_nsec);
            userAmp=procEnd.tms_utime - procStart.tms_utime;
            coreAmp=procEnd.tms_stime - procStart.tms_stime;

            printf("wywołanie: size %d, blocksize %d, wayOfAllock: %d, operations set: %d, numbers of operations %d\n",size,blockSize,wayOfAllock,operation,n);
            printf("Czas rzeczywisty: %lf \n",realAmp);
            printf("Czas użytkownika: %ld \n",userAmp);
            printf("Czas procesora: %ld \n\n",coreAmp);

            return 1;


        } else return -1;

    }else if (wayOfAllock == 2) {

        if (operation == 0){//stworzenie tablicy, usunięcie i dodanie zadanej liczby bloków


            //tu trzeba zaczac mierzyc czasy etc
            clock_gettime(CLOCK_REALTIME,&realStart);
            times(&procStart);
            //inicjalizacja tabliczki
            allockTable2(size,blockSize);
            int i;

            for (i=0;i<size;i++){
                addElem2(i,randominium);
            }

            //dodanie elementow
            for (i=0;i<n;i++){
                addElem2(i%size,"ala ma kota");
            }

            //usuniecie elementow
            for(i=0;i<n;i++){
                removeElem2(i%size);
            }
            clock_gettime(CLOCK_REALTIME,&realEnd);
            times(&procEnd);

            realAmp=(realEnd.tv_sec - realStart.tv_sec )*Miliord+(realEnd.tv_nsec - realStart.tv_nsec);
            userAmp=procEnd.tms_utime - procStart.tms_utime;
            coreAmp=procEnd.tms_stime - procStart.tms_stime;

            printf("wywołanie: size %d, blocksize %d, wayOfAllock: %d, operations set: %d, numbers of operations %d\n",size,blockSize,wayOfAllock,operation,n);
            printf("Czas rzeczywisty: %lf \n",realAmp);
            printf("Czas użytkownika: %ld \n",userAmp);
            printf("Czas procesora: %ld \n\n",coreAmp);

            return 1;


        }else if (operation == 1) {//stworzenie tablicy, wyszukanie elementu oraz usunięcie i dodanie zadanej liczby bloków


            //tu trzeba zaczac mierzyc czasy etc
            clock_gettime(CLOCK_REALTIME,&realStart);
            times(&procStart);
            //inicjalizacja tabliczki
            allockTable2(size,blockSize);
            int i;

            for (i=0;i<size;i++){
                addElem2(i,randominium);
            }

            //dodanie elementow
            for (i=0;i<n;i++){
                addElem2(i%size,"ala ma kota");
            }

            serchClosest2(*randominium2);

            //usuniecie elementow
            for(i=0;i<n;i++){
                removeElem2(i%size);
            }
            clock_gettime(CLOCK_REALTIME,&realEnd);
            times(&procEnd);

            realAmp=(realEnd.tv_sec - realStart.tv_sec )*Miliord+(realEnd.tv_nsec - realStart.tv_nsec);
            userAmp=procEnd.tms_utime - procStart.tms_utime;
            coreAmp=procEnd.tms_stime - procStart.tms_stime;

            printf("wywołanie: size %d, blocksize %d, wayOfAllock: %d, operations set: %d, numbers of operations %d\n",size,blockSize,wayOfAllock,operation,n);
            printf("Czas rzeczywisty: %lf \n",realAmp);
            printf("Czas użytkownika: %ld \n",userAmp);
            printf("Czas procesora: %ld \n\n",coreAmp);

            return 1;

        }else if (operation == 2) {//stworzenie tablicy naprzemienne usunięcie i dodanie zadanej liczby bloków


            //tu trzeba zaczac mierzyc czasy etc
            clock_gettime(CLOCK_REALTIME,&realStart);
            times(&procStart);
            //inicjalizacja tabliczki
            allockTable2(size,blockSize);
            int i;

            for (i=0;i<size;i++){
                addElem2(i,randominium);
            }

            // naprzemienne usunięcie i dodanie elementow
            for (i=0;i<n;i++){
                removeElem2(i%size);
                addElem2(i%size,"ala ma kota");
            }

            clock_gettime(CLOCK_REALTIME,&realEnd);
            times(&procEnd);

            realAmp=(realEnd.tv_sec - realStart.tv_sec )*Miliord+(realEnd.tv_nsec - realStart.tv_nsec);
            userAmp=procEnd.tms_utime - procStart.tms_utime;
            coreAmp=procEnd.tms_stime - procStart.tms_stime;

            printf("wywołanie: size %d, blocksize %d, wayOfAllock: %d, operations set: %d, numbers of operations %d\n",size,blockSize,wayOfAllock,operation,n);
            printf("Czas rzeczywisty: %lf \n",realAmp);
            printf("Czas użytkownika: %ld \n",userAmp);
            printf("Czas procesora: %ld \n\n",coreAmp);

            return 1;


        }else return -1;

    } else return -1;

}







//TESTY
//TESTY
//TESTY
//TESTY
//TESTY



/*
/////////////////////////////////////////////////////////////////////////////////////////////
                            //test1\\
/////////////////////////////////////////////////////////////////////////////////////////////

    char check;
    check = allockTable(20,20); //działa
    printf("%d",check);

    char jeb[20]= "ala ma kota";
    check = addElem(4,jeb);
    check = addElem(5,jeb);
    check = addElem(6,jeb);
    char* xD;
    xD = get(4);
    printf(xD); //wypisywanie elemntu działa
    xD = get(4);
    printf(xD);

    removeElem(4);//usuwanie działa
    printf(xD);


    jeb[20]= "ala ma koty";
    check = addElem(11,jeb);

    jeb[20]= "ala ma kota3231312";
    check = addElem(12,jeb);
    jeb[20]= "ala ma kotafdsafds";
    check = addElem(13,jeb);
    jeb[20]= "ala ma kotadfafdsa";
    check = addElem(14,jeb);

    int z;
    z = serchClosest(11);//działa
    xD = get(z);
    printf(" dasdas#");
    printf("%d",z);
    printf(xD);
    printf(" #dasdas ");


    printf("\n%d       %d        %p\n",hue.size,hue.sizeOfBlock,hue.table);
    check=freeTable(); //tez działa
    printf("\n%d       %d        %p\n",hue.size,hue.sizeOfBlock,hue.table);


    printf("\n\n\n\n\n###############################################################################END OF TEST 1\n############################################################################### \n\n\n\n");
/////////////////////////////////////////////////////////////////////////////////////////////
                            //test2\\
/////////////////////////////////////////////////////////////////////////////////////////////
    char check;
    char jeb[20]= "ala ma kota";
    char* xD;


    check = allockTable2(20,20); //NIE działa
    printf("\n%d       %d        %p\n",hue2.size,hue2.sizeOfBlock,hue2.table);

    check = addElem2(4,jeb); //cos sie sypie
    xD = get2(4);
    printf("%s",xD);

    check = addElem2(7,jeb);
    xD = get2(7);
    printf("%s",xD);

    xD = get2(5);
    printf("%s",xD);

    check = removeElem2(4);
    xD = get2(4);
    printf("%s",xD);


    jeb[20]= "ala ma koty";
    check = addElem2(11,jeb);

    jeb[20]= "ala ma kota3231312";
    check = addElem2(12,jeb);
    jeb[20]= "ala ma kotafdsafds";
    check = addElem2(13,jeb);
    jeb[20]= "ala ma kotadfafdsa";
    check = addElem2(14,jeb);



    int z;
    z = serchClosest2(7);
    printf("\n\n\n\n>%d\n\n",z);
    xD = get(z);
    printf(" dasdas777#");
    printf("%d",z);
    printf(xD);
    printf(" #dasdas777 ");




    check=freeTable2();
    printf("\n%d       %d        %p\n",hue2.size,hue2.sizeOfBlock,hue2.table);
   // printf("%d",check);


    printf("\n\n\n\n\n###############################################################################END OF TEST 2\n############################################################################### \n\n\n\n");

*/

