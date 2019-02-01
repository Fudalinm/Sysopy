#include <stdio.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 

#include "header.h"

mqd_t privateQueue = -10;
mqd_t publicQueue  = -10;
char cPath[40];

int ID = -1;

void exMirror(Msgbuff* msg){
	msg->msg_type = MIRROR;
	printf("podaj linijke do exMirrora\n");
	if( fgets(msg->msg_txt, sizeof(msg->msg_txt) , stdin) == NULL ){ printf("\nnie udalo nam sie pobrac w exMirror\n");return;}
	printf("od uzyszkodnika otrzymałem \n:%s\n",msg->msg_txt );
	if(mq_send(publicQueue,(char*) msg , MSG_SIZE, 1) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exMirror"); return;}
	if(mq_receive(privateQueue,(char*) msg , MSG_SIZE, NULL ) == -1){printf("nie udalo nam sie odebrac nic od servera w exMirror\n"); return;}
	printf("od servera otrzymałem \n:%s\n",msg->msg_txt );
}
void exCalc(Msgbuff* msg){
	msg->msg_type = CALC;
	printf("podaj linijke do exCALC\n");
	if( fgets(msg->msg_txt, sizeof(msg->msg_txt) , stdin) == NULL ){printf("\nnie udalo nam sie pobrac w exCALC\n");}
	printf("od uzyszkodnika otrzymałem \n:%s\n",msg->msg_txt );
	if(mq_send(publicQueue,(char*) msg , MSG_SIZE, 1) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exCalc"); return;}
	if(mq_receive(privateQueue,(char*) msg , MSG_SIZE, NULL ) == -1){printf("nie udalo nam sie odebrac nic od servera w exCALC\n"); return;}
	printf("od servera otrzymałem \n:%s\n",msg->msg_txt ); //zastanowic sie czy na pewno tak to printuje
}
void exTime(Msgbuff* msg){
	msg->msg_type = TIME;
	printf("podaj linijke do exTime");
	if( fgets(msg->msg_txt, sizeof(msg->msg_txt) , stdin)  == NULL ){ printf("\nnie udalo nam sie pobrac w exTime\n");}
	printf("od uzyszkodnika otrzymałem \n:%s\n",msg->msg_txt );
	if( mq_send(publicQueue,(char*) msg , MSG_SIZE, 1) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exTime"); return;}
	if(mq_receive(privateQueue,(char*) msg , MSG_SIZE, NULL ) == -1 ){printf("nie udalo nam sie odebrac nic od servera w exTime\n"); return;}
	printf("od servera otrzymałem \n:%s\n",msg->msg_txt );
}
void exEnd(Msgbuff* msg){
	msg->msg_type = END;
	if(mq_send(publicQueue, (char*) msg , MSG_SIZE, 1) == -1 ){printf("nie udalo nam sie wyslac wiadomosci w exTime"); return;}
}

//to powinno pyknac
void fnExit(void){
	printf("jestem w fnExit\n");
	
	mq_close(publicQueue);
	mq_close(privateQueue);
	mq_unlink(cPath);

}


//dorobic atexit i wszelkie brzydkie returny na exitFailure....
int main(){
	atexit(fnExit);
	pid_t PID = getpid();
	
	sprintf(cPath, "/%d", PID);
	
	//otwieramy kolejki
	
	struct mq_attr atrybuty;
	atrybuty.mq_maxmsg  = MAX_MSG;
	atrybuty.mq_msgsize = MSG_SIZE;
	
	publicQueue = mq_open(PATH,O_WRONLY);
	privateQueue = mq_open(cPath,O_RDONLY | O_CREAT | O_EXCL, 0666 ,&atrybuty);
	
	Msgbuff msg;
	
	//rejstrujemy sie 
	msg.pid = PID;
	msg.msg_type = SIGN;
	sprintf(msg.msg_txt, "%s" , cPath );
	
	if(mq_send(publicQueue,(char*) &msg , MSG_SIZE, 1) == -1 ) { printf("nie udalo nam sie wyslac do publiczej kolejki\n"); return 0;}
	if(mq_receive(privateQueue,(char*) &msg , MSG_SIZE, NULL) == -1 ) { printf("nie udalo nam sie odebrac nic od servera\n"); return 0;}
	if(sscanf(msg.msg_txt, "%d" , &ID) < 0 ) { printf("nie udalo sie odczytac id"); return 0;}
	printf("Nasza sesja ma id: %d\n",ID);
	
	//koniec rejstrowania	
	
	//zaczynamy spamic do servera//
	while(1){
		msg.pid = PID;
		printf("\n Podaj komende!(tylko komende)\n");
		char buf[50];
		if( fgets(buf, sizeof(buf) , stdin)  == NULL ){
			printf("\nnie udalo nam sie wczytc tej linijki foch\n");
		}else if( strstr( buf , "MIRROR" ) != NULL  ){
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





















