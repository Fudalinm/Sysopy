#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 

#include <string.h>

#include "header.h"

int stillProc = 1;
int publicQueue = -10;
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
	
	if(msgsnd(cQueue, msg , MSG_SIZE, 0) == -1 ){printf("Nie udalo nam sie wyslac wiadomosci w exMirror"); return;}
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
	
	if(msgsnd(cQueue, msg , MSG_SIZE, 0) == -1 ){printf("Nie udalo nam sie wyslac wiadomosci w exCalc"); return;}
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
	
	if(msgsnd(cQueue, msg , MSG_SIZE, 0) == -1 ){printf("Nie udalo nam sie wyslac wiadomosci w exCalc"); return;}
	return;
	
}

void exEnd (Msgbuff* msg){
	stillProc = 0;
	return;
}

void exSign (Msgbuff* msg){
	
	key_t cKey;
	pid_t cPid;
	
	if(sscanf(msg->msg_txt, "%d",&cKey) <0) {printf("nie odczytalem klucza\n"); return;}
	cPid = msg->pid;
	
	int cQueue = msgget(cKey , 0);
	if( cQueue == -1) {printf("nie udalo nam sie otworzyc kolejeczki\n");return;}
	
	sprintf(msg->msg_txt,"%d",nextId);
	msg->pid = getpid();
	msg->msg_type = SIGN;
	
	if(msgsnd(cQueue, msg , MSG_SIZE, 0) == -1 ){printf("nie udalo nam sie wyslac wiadomosci do nowego klienta"); return;}

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
	if(publicQueue != -10 ){
			if( msgctl(publicQueue, IPC_RMID, NULL) == -1 ){
				printf("Zamykamy kolejkę\n");
			}else{
				printf("nie udalo nam sie zamknąc kolejki");
			}
	}
}


int main(){
	atexit(exitFun);
	int i;
	for(i=0;i<MAX_CLIENTS;i++){
		privateQueues[i][0] = -2; // tutaj zapisujemy nasze PID
		privateQueues[i][1] = -2; // tutaj zapisujemy id kolejki
	}
	char* path = getenv(PATH);
		
	key_t publicKey = ftok(path,PR_ID);
	if(publicKey == -1) {printf("nie udalo sie wygenerowac klucza publicznego\n");}
	
	
	if (path == NULL) {printf("nie udalo sie pobrac sciezki"); return 0;}
	
	publicQueue = msgget( publicKey , IPC_CREAT | IPC_EXCL | 0666 ); //co to za opcje te po prawej stronie???
	if(publicQueue == -1) {printf("nie udalo sie stworzyc kolejki publicznewj"); return 0;}
	
	Msgbuff msg;
	struct msqid_ds state;
	
	while(1){
		if(stillProc == 0){
			if(msgctl(publicQueue, IPC_STAT, &state) == -1) printf("nie udalo sie zdobyc stanu kolejki publicznej\n"); return 0;;
            if(state.msg_qnum == 0) break;	//jesli kolejka jest pusta to wychodzimy
		}
			if(msgrcv(publicQueue, &msg , MSG_SIZE, 0, 0) == -1 ) { printf("nie udalo nam sie odebrac nic w publicznej kolejce\n"); return 0;}
			execCommand(&msg);
	}
	return 0;
}
