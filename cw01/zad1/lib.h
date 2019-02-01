#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED
struct allocatedDate //struktura przechowujaca rozmiar bloków rozmiar calej tablicy oraz wskaznik na tablice wlasciwa
{
    void* table;
    int sizeOfBlock;
    int size;
};

char allockTable (int size, int maxLength);
char freeTable();
char addElem (int index,char* data);
char removeElem (int index);
int serchClosest (int index);
char* get (int index);

int allockTable2(int size, int sizeOfBlock);
int freeTable2 ();
int addElem2(int index,char* data);
int removeElem2(int index);
int serchClosest2(int index);
char* get2 (int index);

int absDiff (int k, int l);


#endif // LIB_H_INCLUDED

