#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#define true 1
#define false 0

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

extern struct allocatedDate hue = {NULL,0,0}; //jak zainicjalizowac wiecej tablic?
extern struct allocatedDate hue2 = {NULL,0,0};

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

char allockTable (int size, int maxLength) //ilosc oraz rzmiar blokow
{
    if(hue.table != NULL) return false;

    void * table = calloc(size,maxLength);
    if (table == NULL) return false;

    hue.table = table;
    hue.size = size;
    hue.sizeOfBlock = maxLength;

    return true;
}

char freeTable(){
    if (hue.table == NULL) return false;
    free(hue.table);

    hue.table = NULL; hue.size = 0; hue.sizeOfBlock = 0;
    return true;
}


char addElem (int index,char* data){//tez aktualizuje
    if (hue.table == NULL || hue.size <= index || index < 0) return false;
    char* tmp = hue.table;
    tmp += index*hue.sizeOfBlock;
                //void *memcpy (void* dest, const void* src, size_t size);
    void* tmp2 = memcpy (tmp, data,hue.sizeOfBlock); // !!!!!!!!!!!!!!!
    if (tmp2 == NULL) return false;
    return true;
}


char removeElem (int index){
    if (hue.table == NULL) return false;
    char* tmp = hue.table;
    tmp += index*hue.sizeOfBlock;

    int i;
    for(i=0; i<hue.sizeOfBlock;i++){
        *tmp = '\0';
        tmp+=1;
    }
    return true;
}

int serchClosest (int index){
    if (hue.table == NULL) return -1;
    char* tmp = hue.table;
    tmp += index*hue.sizeOfBlock;

    int oSum = 0;
    int i;
    for(i=0; i<hue.sizeOfBlock;i++){
        oSum += *tmp;
        tmp+=1;
    }


    char* serched = hue.table;
    int closestSerched = 2147483646;
    int closestIndex =-2;


    for (i=0; i<hue.size;i++){
        if(i==index){ serched+=hue.sizeOfBlock;continue;}

        int tmpSum =0;
        int j=0;

        for (j=0;j<hue.sizeOfBlock;j++){
            tmpSum += (*serched);
            serched += 1;
        }
         if (absDiff(tmpSum,oSum) < closestSerched ){
            closestIndex = i;
            closestSerched = absDiff(tmpSum,oSum);
         }
    }
    return closestIndex;
}

char* get (int index){
     if (hue.table == NULL || hue.size <= index || index < 0) return false;
    return ((char*) hue.table + index*hue.sizeOfBlock);

}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


int allockTable2(int size, int sizeOfBlock){
    if(hue2.table != NULL) return false;

    hue2.size = size;
    hue2.sizeOfBlock = sizeOfBlock;
    hue2.table = calloc( (sizeof (void*) ) ,size);
    if (hue2.table == NULL) return false;

    char** ptr;
    ptr= hue2.table;


    //w tym forze jest cos zjebane!!!!
    int i=0;
    for (i=0; i<size;i++){
         *ptr = NULL; //dokladnie to tu
         ptr++;
    }


    return true;
}

int freeTable2 (){

    if(hue2.table == NULL) return false;

    char* tmp = hue2.table;
    int i=0;
    for(i=0;i<hue2.size;i++){
        free (*tmp);
        tmp++;
    }
    free(hue2.table);
    hue2.size = 0;
    hue2.table = NULL;
    hue2.sizeOfBlock = 0;
    return true;
}

int addElem2(int index,char* data){

    if (hue2.table == NULL) return false;
    char** tmp = hue2.table;
    tmp+=index;
    char *ptr=malloc(hue2.sizeOfBlock);
    *tmp=ptr;
    void* tmp2 = memcpy(*tmp ,data , hue2.sizeOfBlock);// !!!!!!!!!!!!!!!
    if(tmp2==NULL) return false;
    return true;
}

int removeElem2(int index){
    if (hue2.table == NULL) return false;
    //szukamy zadanego indeksu
    char** tmp = hue2.table;
    tmp += index;
    //wypelniamy dane miejsce nullami
    free(*tmp);
    *tmp=NULL;
    return true;
}

int serchClosest2(int index){


    if (hue2.table == NULL) return -1;

    char** tmp = hue2.table;
    tmp += index;
    int oSum=0;
    int i;
    for(i=0;i<hue2.sizeOfBlock;i++){
        oSum += (int) *( (*tmp)+i);
    }

    char** serched = hue2.table;
    int closesestSerched = 2147483646;
    int closestIndex = -2;

    for(i=0;i<hue2.size;i++){
        if(i==index || *serched == NULL){serched ++; continue;}

        int tmpSum =0;
        int j=0;

        //

        for (j=0;j<hue2.sizeOfBlock;j++){
            tmpSum += (int) *( (*serched) + j);
        }

        if (absDiff(tmpSum,oSum) < closesestSerched ){
            closestIndex = i;
            closesestSerched = absDiff(tmpSum,oSum);
         }
    }
    return closestIndex;
}

char* get2 (int index){
    if (hue2.table == NULL || hue2.size <= index || index < 0) return false;
    char** tmp;
    tmp=hue2.table;
    tmp+=index;
    return *tmp;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

int absDiff (int k, int l){
    int tmp = k-l;
    if (tmp>0) return tmp;
    return -tmp;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


