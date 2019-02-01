#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

//jezeli chcialbym to fajnie napisac to powinny inkrementowac indeksy a dopiero potem sie zajmowac ale
//to spowoduje troche problemow bo kazda komorka stringa powinna miec wtedy swojego mutexa bo 2 klientow
//mogloby chciec dobrac sie do jednej komorki

typedef struct GlobalBuff{
    char** Strings;
    int size;
    int NextToUpload;
    int NextToSearch;
}GlobalBuff;

typedef struct Arguments{
    GlobalBuff* GB;
    pthread_mutex_t* mutex;
}Arguments;

int P = -1;     //ilos producentow
int K = -1;     //ilosc konsumentow
int N = -1;     //ilosc wskaznikow na stringi
char* Inf = NULL; FILE* InFD; //nazwa pliku i jego deskryptoe
int L = -1;     //dlugosc zadanego napisu
int SF = -1;    //  -1 => szukamy mniejszych 0 => szukamy rownych 1 => szukamy wiekszych
int WI = -1;    //  1 => wszyscy 2 => tylko klienci 3=>nikt
int nk = -1;    //podana liczba sekund

int PK = -1;

pthread_t* producers = NULL;
pthread_t* consumers = NULL;

void raisProducerKill (){
    if(PK == -1){PK ++;}
    exit(0);
}

void raiseConsumerKill (){
	
    if(PK != -1){sleep(1);kill(getpid() , SIGTERM);exit(0);}
    return;
    
}


void* producer(void* ptr){
    //ustawiamy maske watku
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGTERM);
    if(0!=pthread_sigmask(SIG_BLOCK,&set,NULL)) {printf("producer: nie udalo sie ustawic maski sygnalow\n");exit(1);}


    Arguments* argument = (Arguments*) ptr;
   // char buf[512];

    while(1){
		char* buf = malloc(512*sizeof(char));

        if(WI == 1){printf("Producent %lu: Czekam na wziecie mutexu\n ",pthread_self() );}
        if(0 != pthread_mutex_lock(argument->mutex)) {printf("Nie udalo sie zablokowac mutexa w producencie\n");}
        if(WI == 1){printf("Producent %lu: Wzialem Mutex\n ",pthread_self() );}
        
        //zajety jest caly gdy miejsce do ktorego chce wpisac nie jest NULLEM
        if(WI == 1){printf("Producent %lu: Sprawdzam czy jest wolne miejsce do wpisania linii\n",pthread_self() );}
        if(argument->GB->Strings[argument->GB->NextToUpload] != NULL){ //wtedy nie mamy gdzie wpisac
            if(WI == 1){printf("Producent %lu: Nie ma gdzie wpisac wiec zwalniam Mutex\n",pthread_self() );}
            if(0 != pthread_mutex_unlock(argument->mutex)){printf("Nie udalo sie odblokowac mutexa w producencie\n");}
            continue;
        }
        //wiemy ze jest miejsce do wpisania
        if(WI == 1){printf("Producent %lu: jest wolne miejsce do wpisania danych\n Pobieram linie z pliku wejsciowego\n",pthread_self() );}
        if(NULL == fgets(buf,512,InFD)){
            if(WI == 1){printf("Producent %lu: Plik skonczyl sie koncze prace\n",pthread_self() );}
            printf("Nie udalo sie wczytac lini albo skonczylismy plik\n");
            printf("Wznosimy sygnal zakonczenia pracy\n");
			if(0 != pthread_mutex_unlock(argument->mutex)){printf("Nie udalo sie odblokowac mutexa w producencie\n");}
            raisProducerKill();
        }
		//zakladamy ze skonczyl sie plik wtedy co???
		//printf("xDDD: %s",buf);
        if(WI == 1){printf("Producent %lu: Zapisuje zadana linie do naszego buffora\n",pthread_self() );}
        argument->GB->Strings[argument->GB->NextToUpload] = buf;
		
        argument->GB->NextToUpload++;
        if(argument->GB->NextToUpload == argument->GB->size){argument->GB->NextToUpload = 0;}

        if(WI == 1){printf("Producent %lu: odblokowywuje mutexa\n",pthread_self() );}
        if(0 != pthread_mutex_unlock(argument->mutex)){printf("Nie udalo sie zablokowac mutexa w producencie\n");}
    }
}

int LBufCmp(char* buf){
    if(strlen(buf) > L) return 1;
    if(strlen(buf) < L ) return -1;
    return 0;
}

void* consumer(void* ptr){
    //ustawiamy maske watku
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGTERM);
    if(0!=pthread_sigmask(SIG_BLOCK,&set,NULL)) {printf("consumer: nie udalo sie ustawic maski sygnalow\n");exit(1);}


    Arguments* argument = (Arguments*) ptr;
    

    while(1){
		char* buf;
        if(WI != 3){printf("KLIENT %lu: Czekam na wziecie mutexu\n ",pthread_self() );}
        if(0 != pthread_mutex_lock(argument->mutex)) {printf("Nie udalo sie zablokowac mutexa w kliencie\n");}

        if(WI != 3){printf("KLIENT %lu: Sprawdzam czy jest cos do pobrania\n ",pthread_self() );}
        if(argument->GB->Strings[argument->GB->NextToSearch] == NULL){
            if(WI != 3){printf("KLIENT %lu:Nie ma nic do pobrania\n ",pthread_self() );}
            //skoro tak to moze mam skonczyc prace
			if(0 != pthread_mutex_unlock(argument->mutex)){printf("Nie udalo sie odblokowac mutexa w kliencie\n");}
            raiseConsumerKill();
            continue;
        }

        if(WI != 3){printf("KLIENT %lu:Jest coś do pobrania wiec sprawdzamy\n ",pthread_self() );}
        buf = argument->GB->Strings[argument->GB->NextToSearch];
      
		//printf("\nbuf:%s \nargument:%s\n",buf,argument->GB->Strings[argument->GB->NextToSearch]);
		
        if(LBufCmp(buf) == SF){
            printf("KLIENT %lu:Znalazlem fragment speniajacy warunek.\n     Byl w indeksie o numerze: %d\n       A napis to: %s",pthread_self(),argument->GB->NextToSearch,buf);
        }
		
		argument->GB->Strings[argument->GB->NextToSearch] = NULL;


        argument->GB->NextToSearch++;
        if(argument->GB->NextToSearch == argument->GB->size){argument->GB->NextToSearch = 0;}


        if(WI != 3){printf("KLIENT %lu: oddaje mutex\n ",pthread_self() );}
        if(0 != pthread_mutex_unlock(argument->mutex)){printf("Nie udalo sie odblokowac mutexa w kliencie\n");}
    }
}




int loadData(char* buf){
    char* token;
    token = strtok(buf," ");   if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego1\n");exit(1);}         P = atoi(token);
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego2\n");exit(1);}         K = atoi(token);
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego3\n");exit(1);}         N = atoi(token);
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego4\n");exit(1);}         Inf = token;
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego5\n");exit(1);}         L = atoi(token);
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego6\n");exit(1);}         SF = atoi(token);
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego7\n");exit(1);}         WI = atoi(token);
    token = strtok(NULL," ");  if(token == NULL){printf("Nie udalo sie pobrac argumentow z pliku konfiguracyjnego8\n");exit(1);}         nk = atoi(token);
    return 1;
}

void exitfun(){
    //trzeba zakonczyc kazdy watek
    printf("Koncze kazdy watek\n");
    int i;
	
    for(i=0;i<P;i++){
        pthread_cancel(producers[i]);
    }
    for(i=0;i<K;i++){
        pthread_cancel(consumers[i]);
    }
    //zamkncac pliki
	if(InFD){
		fclose(InFD);
	}
}

void exitfun2(int sig){
	exit(0);	
}

int main(int argc,char* argv[]) {

    if(argc != 2){printf("Podaj scieżke do pliku koniguracyjnego\n"); exit(1);}
    atexit(exitfun);
	
	signal(SIGTERM,exitfun2);

    FILE* fp;
    fp = fopen(argv[1],"r");
    if(fp == NULL){printf("nie udalo sie otworzyc pliku\n");exit(1);}
    char buf[256];
    if( 1>fread(&buf,sizeof(char),256,fp)) {printf("nie udalo sie przeczytac pliku konfiguracyjnego\n"); exit(1);}
    loadData(buf);
    fclose(fp); //zamykamy plik to wczytywania danych

	printf("Pobrane dane:%d %d %d %s %d %d %d %d\n",P,K,N,Inf,L,SF,WI,nk);
	//dane wczytuje dobrze


    int i;
    //Strings = (char*) malloc(N*sizeof(char*)); //mamy n wskaznikow
    // teraz chce zeby wskazywal na N char*
    GlobalBuff GBuff;
    GBuff.Strings =  malloc(N* sizeof(char*));
    for(i=0;i<N;i++){GBuff.Strings[i] = NULL;}
    //  GBuff.free = 1;
    GBuff.NextToUpload = 0;
    GBuff.NextToSearch = 0;
    GBuff.size = N;

    //musze zapisac kazdy watek

    producers = malloc(P* sizeof(pthread_t));
    consumers = malloc(K* sizeof(pthread_t));

    //musimy otworzyc plik do czytania lini
    InFD = fopen(Inf,"r");
    if(InFD == NULL){printf("nie udalo sie otworzyc pliku z zasobami do odczytu\n"); exit(1);}

    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_DEFAULT);
    pthread_mutex_init(&mutex,&attr);

    Arguments args;
    args.GB = &GBuff;
    args.mutex = &mutex;


    pthread_mutex_t Endmutex;
    pthread_mutex_init(&Endmutex,&attr);


    for(i=0;i<P;i++){
        pthread_create(&producers[i],NULL,producer, (void *) &args);
    }
    for(i=0;i<K;i++){
        pthread_create(&consumers[i],NULL,consumer,(void *) &args);
    }

    if(nk>0){sleep(nk); exit(0);} //czekamy nk sekund albo na sygnal
	
	//printf("\n\nOczekuje na pasue\n\n\n	");
	pause();
	exit(0);

}
