#include <stdio.h>
#include <stdlib.h>

int main()
{
    long x=0;
    int c = 0;
    while(1){
        if(x%61470000 == 0){
            c++;
            printf("Wypisuje sie juz %d raz!\n",c);
        }
        x++;
    }

}
