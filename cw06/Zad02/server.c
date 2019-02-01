#include <stdio.h>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 
#include <fcntl.h>

#include <string.h>

#include "header.h"

int stillProc = 1;
mqd_t publicQueue = -10;
int nextId = 0; //MAX_CLIENTS 20
int privateQueues[MAX_CLIENTS][2]; // PID || id kolejki

int getCId (Msgbuff* msg){
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(msg->pid == privateQueues[i][0]) {return i;}		
	}
	return -1;
}

void exMirror(Msgbuff* msg){
	
	//zeby wyslac musimy mic kolejeczke
	int cId = getCId(msg); if (cId == -1) {printf("nie udalo sie zobyc id klienta\n");}
	int cQueue = privateQueues[cId][1];

	int length = (int) strlen(msg->msg_txt);
	
	int i;
	for(i=0;i<length/2;i++){
		char buf = msg->msg_txt[i];	
		 msg->msg_txt[i] = msg->msg_txt[length - 1 - i];
		 msg->msg_txt[length - 1 - i] = buf;

	}
	msg->pid = getpid();
	msg->msg_type = MIRROR;
	 
	if(mq_send(cQueue, (char*) msg , MSG_SIZE, 1) == -1 ){printf("Nie udalo nam sie wyslac wiadomosci w exMirror"); return;}
	return;
}

void exCalc (Msgbuff* msg){//commandline
	char buf[MAX_MSG_SIZE];
	
	int cId = getCId(msg); if (cId == -1) {printf("nie udalo sie zobyc id klienta\n");}
	int cQueue = privateQueues[cId][1];
	
	sprintf(buf,"echo '%s' | bc",msg->msg_txt);
	FILE* xD;
	xD = popen(buf , "r");
	if (xD == NULL) {printf("nie umiem liczyc\n");return;}
	fgets(buf,MAX_MSG_SIZE,xD);
	if(pclose(xD) == -1) {printf("nie dalo rady zamknac\n"); return;}
	
	sprintf(msg->msg_txt,"%s", buf );
	msg->pid = getpid();
	msg->msg_type = CALC;
	
	if(mq_send(cQueue, (char*) msg , MSG_SIZE, 1) == -1 ){printf("Nie udalo nam sie wyslac wiadomosci w exCalc"); return;}
	return;
}

void exTime (Msgbuff* msg){//pewnie tez commandline1
	char buf[MAX_MSG_SIZE];
	
	int cId = getCId(msg); if (cId == -1) {printf("nie udalo sie zobyc id klienta\n");}
	int cQueue = privateQueues[cId][1];
	
	FILE* xD;
	xD = popen("date" , "r");
	if (xD == NULL) {printf("nie umiem liczyc\n");return;}
	fgets(buf,MAX_MSG_SIZE,xD);
	if(pclose(xD) == -1) {printf("nie dalo rady zamknac\n"); return;}
	
	sprintf(msg->msg_txt,"%s", buf );
	msg->pid = getpid();
	msg->msg_type = CALC;
	
	if(mq_send(cQueue,(char*) msg , MSG_SIZE, 1) == -1 ){printf("Nie udalo nam sie wyslac wiadomosci w exCalc"); return;}
	return;
	
}

void exEnd (Msgbuff* msg){
	stillProc = 0;
	return;
}

void exSign (Msgbuff* msg){

	char cPath[40];
	pid_t cPid;
	
	if(sscanf(msg->msg_txt, "%s",cPath) <0) {printf("nie odczytalem sciezynki\n"); return;}
	cPid = msg->pid;

	int cQueue = mq_open(cPath,O_WRONLY);  
	if( cQueue == -1) {printf("nie udalo nam sie otworzyc kolejeczki\n");return;}
	
	sprintf(msg->msg_txt,"%d",nextId);
	msg->pid = getpid();
	msg->msg_type = SIGN;
	
	if(mq_send(cQueue,(char*) msg , MSG_SIZE, 1) == -1 ){printf("nie udalo nam sie wyslac wiadomosci do nowego klienta"); return;}

	privateQueues[nextId][0] = cPid; //tu pid
	privateQueues[nextId][1] = cQueue;	//tu id kolejki
	
	nextId ++;
	return;
}

void execCommand (Msgbuff* msg){
	switch (msg->msg_type){
		case MIRROR:
			printf("wchodze do mirrora typ: %ld\n",msg->msg_type);
			exMirror(msg);
			break;
		
		case CALC:
			printf("wchodze do calca typ: %ld\n",msg->msg_type);
			exCalc(msg);
			break;
		
		case TIME:
			printf("wchodze do timea typ: %ld\n",msg->msg_type);
			exTime(msg);
			break;
		
		case END:
			printf("wchodze do enda typ: %ld\n",msg->msg_type);
			exEnd(msg);
			break;
		
		case SIGN:
			printf("wchodze do singna typ: %ld\n",msg->msg_type);
			exSign(msg);
			break;
			
		default:
			printf("nie udalo nam sie dopasowac typu: %ld\n",msg->msg_type);
			break;	
	}
	return;
}

void exitFun (void){
	printf("jestem w exitFun\n");
	
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		if(privateQueues[i][0] == -2) {break;}
		if( mq_close(privateQueues[i][1]) == -1 ){
				printf("nie udalo nam sie zamknąc kolejki klienta %d\n",i);
			}
	}	
	if(publicQueue != -10 ){
			if( mq_close(publicQueue) == -1 ){
				printf("nie udalo nam sie zamknąc kolejki\n");
			}else{
				printf("Zamykamy kolejkę\n");
			}
		mq_unlink(PATH);
	}
}


int main(){
	atexit(exitFun);
	
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		privateQueues[i][0] = -2; // tutaj zapisujemy nasze PID
		privateQueues[i][1] = -2; // tutaj zapisujemy id kolejki
	}
	//moze jakies atrybuciki
	struct mq_attr atrybuty;
	atrybuty.mq_maxmsg  = MAX_MSG;
	atrybuty.mq_msgsize = MSG_SIZE;
	
	//otwieramy kolejke POSIX
	publicQueue = mq_open(PATH,O_RDONLY | O_CREAT | O_EXCL, 0666, &atrybuty);
	if(publicQueue == -1) {printf("nie udalo sie stworzyc kolejki publicznewj\n"); return 0;}
	//
	
	Msgbuff msg;
	struct mq_attr state;
	
	while(1){
		if(stillProc == 0){
			if(mq_getattr(publicQueue, &state) == -1) printf("nie udalo sie zdobyc stanu kolejki publicznej\n"); return 0;
            if(state.mq_curmsgs == 0) break;	//jesli kolejka jest pusta to wychodzimy
		}
			if(mq_receive(publicQueue, (char*) &msg , MSG_SIZE, NULL) == -1 ) { printf("nie udalo nam sie odebrac nic w publicznej kolejce\n"); return 0;}
			execCommand(&msg);
	}
	return 0;
}
 