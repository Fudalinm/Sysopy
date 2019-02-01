#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 

#include "header.h"

int privateQueue = -10;
int publicQueue  = -10;
int ID = -1;

void exMirror(Msgbuff* msg){
	msg->msg_type = MIRROR;
	printf("podaj linijke do exMirrora\n");
	if( fgets(msg->msg_txt, sizeof(msg->msg_txt) , stdin) == NULL ){ printf("\nnie udalo nam sie pobrac w exMirror\n");return;}
	printf("od uzyszkodnika otrzymałem \n:%s\n",msg->msg_txt );
	if(msgsnd(publicQueue, msg , MSG_SIZE, 0) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exMirror"); return;}
	if(msgrcv(privateQueue, msg , MSG_SIZE, 0, 0) == -1 ) { printf("nie udalo nam sie odebrac nic od servera w exMirror\n"); return;}
	printf("od servera otrzymałem \n:%s\n",msg->msg_txt );
}
void exCalc(Msgbuff* msg){
	msg->msg_type = CALC;
	printf("podaj linijke do exCALC\n");
	if( fgets(msg->msg_txt, sizeof(msg->msg_txt) , stdin) == NULL ){printf("\nnie udalo nam sie pobrac w exCALC\n");}
	printf("od uzyszkodnika otrzymałem \n:%s\n",msg->msg_txt );
	if(msgsnd(publicQueue, msg , MSG_SIZE, 0) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exCalc"); return;}
	if(msgrcv(privateQueue, msg , MSG_SIZE, 0, 0) == -1 ) { printf("nie udalo nam sie odebrac nic od servera w exCALC\n"); return;}
	printf("od servera otrzymałem \n:%s\n",msg->msg_txt ); //zastanowic sie czy na pewno tak to printuje
}
void exTime(Msgbuff* msg){
	msg->msg_type = TIME;
	printf("podaj linijke do exTime");
	if( fgets(msg->msg_txt, sizeof(msg->msg_txt) , stdin)  == NULL ){ printf("\nnie udalo nam sie pobrac w exTime\n");}
	printf("od uzyszkodnika otrzymałem \n:%s\n",msg->msg_txt );
	if(msgsnd(publicQueue, msg , MSG_SIZE, 0) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exTime"); return;}
	if(msgrcv(privateQueue, msg , MSG_SIZE, 0, 0) == -1 ) { printf("nie udalo nam sie odebrac nic od servera w exTime\n"); return;}
	printf("od servera otrzymałem \n:%s\n",msg->msg_txt );
}
void exEnd(Msgbuff* msg){
	msg->msg_type = END;
	if(msgsnd(publicQueue, msg , MSG_SIZE, 0) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exTime"); return;}
}

//to powinno pyknac
void fnExit(void){
	printf("jestem w fnExit\n");
	if(privateQueue != -10 ){
			if( msgctl(privateQueue, IPC_RMID, NULL) == -1 ){
				printf("Zamykamy kolejkę\n");
			}else{
				printf("nie udalo nam sie zamknąc kolejki");
			}
	}
}


//dorobic atexit i wszelkie brzydkie returny na exitFailure....
int main(){
	atexit(fnExit);
	pid_t PID = getpid();
	
	char* path = getenv(PATH);
	if (path == NULL) {printf("nie udalo sie pobrac sciezki"); return 0;}
	
	//tworzymy klucze
	
	key_t publicKey = ftok(path,PR_ID);
	if(publicKey == -1) {printf("nie udalo sie wygenerowac klucza publicznego\n"); return 0;}
	
	key_t privateKey = ftok(path,PID);
	if(privateKey == -1) {printf("nie udalo sie wygenerowac klucza prywatnego\n"); return 0;}
	
	//musze otworzyc nasza publiczna kolejeczke i prywatna :parsfnm
	
	//otwieramy kolejki
	publicQueue = msgget( publicKey , 0); //co to za opcje te po prawej stronie???
	if(publicQueue == -1) {printf("nie udalo sie stworzyc kolejki publicznewj"); return 0;}
	
	privateQueue = msgget( privateKey , IPC_CREAT | IPC_EXCL | 0666 ); //co to za opcje te po prawej stronie???
	if(privateQueue == -1) {printf("nie udalo sie stworzyc kolejki prywatnej"); return 0;}
	
	Msgbuff msg;
	
	//rejstrujemy sie 
	msg.pid = PID;
	msg.msg_type = SIGN;
	sprintf(msg.msg_txt, "%d" , privateKey );
	
	if(msgsnd(publicQueue, &msg , MSG_SIZE, 0) == -1 ) { printf("nie udalo nam sie wyslac do publiczej kolejki\n"); return 0;}
	if(msgrcv(privateQueue, &msg , MSG_SIZE, 0, 0) == -1 ) { printf("nie udalo nam sie odebrac nic od servera\n"); return 0;}
	if(sscanf(msg.msg_txt, "%d" , &ID) < 0 ) { printf("nie udalo sie odczytac id"); return 0;}
	printf("Nasza sesja ma id: %d\n",ID);
	
	//koniec rejstrowania	
	
	//zaczynamy spamic do servera//
	while(1){
		msg.pid = PID;
		
		printf("\n Podaj komende!(tylko komende)");
		char buf[50];
		if( fgets(buf, sizeof(buf) , stdin)  == NULL ){
			printf("\nnie udalo nam sie wczytc tej linijki foch\n");
		}
		
		if( strstr( buf , "MIRROR" ) != NULL  ){
			exMirror(&msg);
		}else if( strstr( buf , "CALC" ) != NULL  ){
			exCalc(&msg);
		}else if( strstr( buf , "TIME" ) != NULL ){
			exTime(&msg);	
		}else if( strstr( buf , "END" ) != NULL ){		
			exEnd(&msg);
		}else if (strstr( buf , "q" ) != NULL) {
			exit(0);			
		} else {
			
			printf("Couldn't match command\n");
		}		
	}

}





















