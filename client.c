/********************************************************************************************************
* @author:- Mohamed Rida HANTI   									*
*         - Mohammed MAHDOUTE              							        *
* @university: UFR Sciences et Techniques     								*
********************************************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
#define READ 0
#define WRITE 1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

void error(const char *msg) {
	perror(msg);
	exit(1);
}

int main(int argc, char ** argv) { 

	int PORT, n, JETON, i, status;
	int pipeParent[2];
	int pipeChild[2];
	pid_t pID;

	SOCKET sock;
	SOCKADDR_IN sin;

	if(argc != 3) {
                fprintf(stderr,"SYNTAXE: ./client [HOST] [PORT]\n");
                exit(1);
        }

	if(pipe(pipeParent) != 0) error("ERREUR: pipe()");
	if(pipe(pipeChild) != 0) error("ERREUR: pipe()");

	/* Création de la socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) error("ERREUR: socket()");

	/* Configuration de la connexion */
	PORT = atoi(argv[2]);
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	
	pID = fork();
	if(pID > 0) {
		if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR) {
			do {
                	        n = recv(sock, &JETON, sizeof(JETON), 0);
                        	if (n < 0) error("ERREUR: recv()");

				if(JETON != -2) {
		                        close(pipeParent[READ]);
        	                        write(pipeParent[WRITE], &JETON, sizeof(JETON));

					close(pipeChild[WRITE]);
		                        read(pipeChild[READ], &JETON, sizeof(JETON));
				
		                        if(((JETON) % 3) == 0) {
        		                        if(JETON > 50) JETON = -2;
                	        	}

	                        	 if(JETON != -2) printf("client> JETON = %d\n", JETON);
				}

	                        n = send(sock, &JETON, sizeof(JETON), 0);
        	                if (n < 0) error("ERREUR: send()");
                	} while(JETON != -2);
			exit(1);
        	} else
                	error("ERREUR: connect()");
	} else if(pID == 0) {
		do {
			close(pipeParent[WRITE]);
			read(pipeParent[READ], &JETON, sizeof(JETON));
			if(JETON != -2) {
//				printf("tube #%d\n> jeton=%d\n", getpid(), JETON);
				JETON++;
				close(pipeChild[READ]);
        	                write(pipeChild[WRITE], &JETON, sizeof(JETON));
			}
		} while(JETON != -2);
		printf("fils mort\n");
		exit(1);
	}
	
	wait(&status);
	/* On ferme la socket précédemment ouverte */
	closesocket(sock); 
    return EXIT_SUCCESS;
}
