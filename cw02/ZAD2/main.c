#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define _XOPEN_SOURCE 500
#include <ftw.h>

char* op;
time_t inDate;



void printStats(struct stat *stata,char* directPath){
    
    printf("\n\n\n");
    printf(" Bezwzgledna scieżka do Pliku: %s \n",directPath);
    
    printf("Prawa do pliku: ");
    printf( (S_ISDIR(stata->st_mode)) ? "d" : "-");
    printf( (stata->st_mode & S_IRUSR) ? "r" : "-");
    printf( (stata->st_mode & S_IWUSR) ? "w" : "-");
    printf( (stata->st_mode & S_IXUSR) ? "x" : "-");
    printf( (stata->st_mode & S_IRGRP) ? "r" : "-");
    printf( (stata->st_mode & S_IWGRP) ? "w" : "-");
    printf( (stata->st_mode & S_IXGRP) ? "x" : "-");
    printf( (stata->st_mode & S_IROTH) ? "r" : "-");
    printf( (stata->st_mode & S_IWOTH) ? "w" : "-");
    printf( (stata->st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    
    printf("Rozmiar pliku(w bajtach): %lld\t\n",stata->st_size);
    
    printf("Data ostatniej modyfikacji: %s\n",ctime(&stata->st_mtime)); /// Ostatnia modyfikacja
    printf("\n\n\n");
    return;
}


int fn ( const char * file, const struct stat * statystyki, struct FTW * s ){
    
    if( (S_ISLNK(statystyki->st_mode) == 0)   ){
        
        if ( strcmp(op,"<") == 0 &&  difftime(statystyki->st_mtime,inDate) < 0 ){
            
            printStats(statystyki,file);
            
        } else if (strcmp(op,">") == 0 &&  difftime(statystyki->st_mtime,inDate) > 0 ){
            
            printStats(statystyki,file);
            
        }else if (strcmp(op,"=") == 0 && difftime(statystyki->st_mtime,inDate) == 0  ){
            
            printStats(statystyki,file);
            
        }else {return 0;}
        
    } else {return 0;}
    
    return 0;
}


int statOpen(char* path,time_t date){
    
    DIR *dir;
    dir = opendir(path);
    if(dir == NULL) {
        return -1;
    }
    
    struct dirent *reader;
    reader = readdir(dir); ///czytamy pierwszy plik z katalogu
    char temporaryPath[1000];
    
    struct stat statystyki;
    
    ///musimy otrzymac pelna sciezke do katalogu/pliku!!!!
    
    while(reader != NULL){///przeszukujemy caly katalog
        
        ///atualizujemy sciezke bezwzgledna
        strcpy(temporaryPath,path);
        char* xD = malloc(sizeof(path)+sizeof(reader->d_name) +    10);
        strcat(xD , "/");
        strcat(xD , reader->d_name);
        strcat(temporaryPath , xD);
        
        char* ptr;
        ptr = temporaryPath;
        
        stat(temporaryPath,&statystyki);
        if(strcmp(xD,"/..") != 0 && strcmp(xD,"/.") != 0 ){
            if( (S_ISLNK(statystyki.st_mode) == 0)   ){ ///jezeli nie jest linkiem to go wypisujemy NALEZY DODAC WARUNEK Z DATA!!! ALE TO SPRAWDZIMY NAD!
                
                if (strcmp(op,"<") == 0 &&  difftime(statystyki.st_mtime,date) < 0 ){
                    
                    printStats(&statystyki,temporaryPath);
                    
                } else if (strcmp(op,">") == 0  &&  difftime(statystyki.st_mtime,date) > 0 ){
                    
                    printStats(&statystyki,temporaryPath);
                    
                }else if (strcmp(op,"=") == 0 && difftime(statystyki.st_mtime,date) == 0  ){
                    
                    printStats(&statystyki,temporaryPath);
                    
                }else if (S_ISDIR(statystyki.st_mode) ){///jezeli mamy przeszukac tez podkatalogi to wywolujemy to rekurencyjnie i tworzymy dodatkowa warstwe funkcyjna zeby w tej nie azmykac katalogu na koniec!
                    statOpen(ptr,date);
                }
            }
        }
        reader = readdir(dir);//czytamy nastepny z plikow
    }
    closedir(dir);
    return 1;
}


time_t parseInDate(int s, int m, int h, int d, int mn, int y){
    
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeinfo->tm_year = y - 1900;
    timeinfo->tm_mon = mn - 1;
    timeinfo->tm_mday = d;
    timeinfo->tm_hour = h;
    timeinfo->tm_min = m;
    timeinfo->tm_sec = s;
    return mktime ( timeinfo );
    
}


int nftwOpen(char* path,time_t date){
    
    int x;
    x = nftw(path,fn,1000,FTW_F);
    return x;
    
}

int main(int argc, char *argv[])
{
    
    
    
    
    
    if(argc == 10){
        char* path;
        char* way;
        
        path = argv[1];
        way = argv[2];
        op=argv[3]; /// < > =
        /// ktory sposob alokacji
        /// przyjmuje od sekund do lat /// w wywolaniu programu podajemy rok miesiac dzien godzina minuta sekundy
        inDate = parseInDate( atoi(argv[9]), atoi(argv[8]), atoi(argv[7]), atoi(argv[6]), atoi(argv[5]), atoi(argv[4]));
        
        
        ///sprrawdzamy czy n czy s
        char* dPath;
        char actualpath [1001];
        
        dPath = realpath(path, actualpath);
        
        if(strcmp(way,"n") == 0){
            
            
            nftwOpen(actualpath,inDate);
            return 0;
            //tu wywolamy po prostu z realpath i bedzie git ;p
            //trzeba sformatwac date jakos
            // statOpen(*path,*operation)
            
            
            
            
        }else if (strcmp(way,"s") == 0){
            
            statOpen(actualpath,inDate);
            return 0;
            
            //tu wywolamy po prostu z realpath i bedzie git ;p
            //trzeba sformatwac date jakos
            // statOpen(*path,*operation)
            
        }else{
            printf("bad arguments");
            return -1;
        }
        
        
        return 1;
        
    }else{
        
        printf("bad arguments");
        return -1;
    }
    
    
}

