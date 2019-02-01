#ifndef HEADER_H
#define HEADER_H

#define MAX_CLIENTS 20
#define MAX_MSG_SIZE 1000

#define MAX_MIRR 1200
#define PR_ID 5372
#define PATH "HOME"


typedef enum msg_type {
	MIRROR 	= 1,
	CALC 	= 2,
	TIME	= 3,
	END 	= 4,
	SIGN  	= 5
}msg_type;

typedef struct Msgbuff {
	pid_t pid;
	long msg_type;
	char msg_txt[MAX_MSG_SIZE];
}Msgbuff;

const size_t MSG_SIZE = sizeof (Msgbuff) - sizeof (long);

#endif

























