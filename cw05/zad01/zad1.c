#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>

#define maxArgs 64
#define maxLine 256
#define maxCommands 50


////////////

char* makeItNice(char* lineToMakeNicer){
	//najpierw pozbywamy sie bialych znakow
	char* buf = malloc(sizeof(char) * maxLine);
	char* Cline = lineToMakeNicer;
	//przewijamy spacje
	while (*Cline == ' ') {Cline++;}
    int k = 0;
	while(*Cline != '\0'){
		   while((*Cline != ' ') && (*Cline != '\0')){
				buf[k++] = *Cline;
				Cline++;
       		}
        if(*Cline == ' '){
            while(*Cline == ' ') {Cline++;}
            if (*Cline != '\0'){
				buf[k++] = ' ';
			}
          }
	}
	buf[k+1] = '\0';
	return buf;
}


char** makeItEvenNicer(char* ugly){
	int size = 0;
	char** toRet = NULL;
	char scars[3] = {'\t','\n',' '}; //usuwamy taby spacje nowe linie

	char* tmp = strtok(ugly,scars);

	while( tmp != NULL ) {
		size++;
		toRet = realloc( toRet, sizeof(char*) * size);
		toRet[size -1] = tmp;
		tmp = strtok(NULL,scars);
	}
	toRet = realloc(toRet, sizeof(char*)* (size + 1) );
	toRet[size] = NULL;
	return toRet;
}


int doIt(char* line){
	int CommandC = 0;
	//wystarcza dwa bo zachowujemy poprzedni nastepny a potem przyszly
	int pipy[2][2]; //niewazne co jest wewnatrz
	char* commands[maxCommands];

    while((commands[CommandC] = strtok(CommandC == 0 ? line : NULL, "|")) != NULL){
        CommandC++;
    }

	int i=0;
	while(i < CommandC) {
	//zamykamy stare niepotrzebne pipy
		if (i > 0) {
            close(pipy[i % 2][0]);
            close(pipy[i % 2][1]);
        }

        if(pipe(pipy[i % 2]) == -1) {
            printf("Error on pipe.\n");
            exit(EXIT_FAILURE);
        }

		pid_t child = fork();
		if(child == 0){
			//przerabiamy wnetrze pipow na trawialne :)
			char* nice = makeItNice(commands[i]);
			char** evenNicer = makeItEvenNicer(nice);
			//jezeli nie jestesmy ostatnim to zamykamy i tworzymy wyjsciowy
			  if ( i  !=  CommandC -1) {
					close(pipy[i % 2][0]);
					if (dup2(pipy[i % 2][1], STDOUT_FILENO) < 0) {
						exit(1);
					}
				}
			//jezeli nie jest pierwszy to zamykamy stare wejscie i otiweramy nowe
				if (i != 0) {
					close(pipy[(i + 1) % 2][1]);
					if (dup2(pipy[(i + 1) % 2][0], STDIN_FILENO) < 0) {
						close(1);
					}
				}

			  execvp(evenNicer[0], evenNicer);
			  exit(0);
		}
		i++;
	}
	close(pipy[i % 2][0]);
    close(pipy[i % 2][1]);
	//czekamy na wszystkie procki
	wait(NULL);
	exit(0);
}



/////////////




int main(int argc, char *argv[]) {


	FILE* f = fopen(argv[1], "r");
	if (!f) {printf("not able to open file"); return 20;}

	char tmp_line[maxLine];
	char* args[maxArgs];

	int argumentC = 0;

	while( fgets( tmp_line , maxLine , f) ){
		argumentC = 0;
		pid_t child = fork();
		if ( child == 0 ){

			doIt(tmp_line);
			exit(1);
		}

		int st;
        wait(&st);
        if (st) {
            printf( "nie udalo sie wywolac\n");
			int i;
            for (i = 0; i < argumentC; i++) {
                printf("%s ", args[i]);
            }
            return 1;
        }
	}
	fclose(f);
	return 0;

}

