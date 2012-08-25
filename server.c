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

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char ** argv) {

	int PORT, n, i, JETON;

	/* Socket et contexte d'adressage du serveur */
	SOCKADDR_IN sin;
	SOCKET sock;
	socklen_t recsize = sizeof(sin);
    
	/* Socket et contexte d'adressage du client */
	SOCKADDR_IN csin[3];
	SOCKET csock[3];
	socklen_t crecsize = sizeof(SOCKADDR_IN);
    
	int sock_err;

	if(argc != 2) {
                fprintf(stderr,"SYNTAXE: ./server [PORT]\n");
                exit(1);
        }
        
	/* Création d'une socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) error("ERREUR: socket()");
	
	PORT = atoi(argv[1]);
	
	/* Configuration */
	sin.sin_addr.s_addr = htonl(INADDR_ANY);  /* Adresse IP automatique */
	sin.sin_family = AF_INET;                 /* Protocole familial (IP) */
	sin.sin_port = htons(PORT);               /* Listage du port */

	if(bind(sock, (SOCKADDR*) &sin, recsize) < 0) error("ERREUR: bind()");

	i = 0;
	while(i < 3) {
		/* Démarrage du listage (mode server) */
		sock_err = listen(sock, 5);
		printf("en attente...\n");

		/* Si la socket fonctionne */
		if(sock_err != SOCKET_ERROR) {
			/* Attente pendant laquelle le client se connecte */
			csock[i] = accept(sock, (SOCKADDR*)&csin[i], &crecsize);
			printf("client #%d connecté\n", i+1);
		} else error("ERREUR: listen()");
		i++;
	}

	i = 0;
	JETON = -1;
	while(JETON > -2) {
		n = send(csock[i], &JETON, sizeof(JETON), 0);
		if (n < 0) error("ERREUR: send()");
      		
		if(JETON != -2) {
			n = recv(csock[i], &JETON, sizeof(JETON), 0);
			if (n < 0) error("ERREUR: read()");
		
			if(JETON != -2) printf("client #%d> %d\n", i+1, JETON);
		}
		i++;

		if(i == 3) i = 0;
	}

	i = 2;
	while(i > -1) {	
		printf("client #%d déconnecté\n", i+1);
        	closesocket(csock[i]);
		i--;
	}

	printf("Fermeture de la socket serveur\n");
	closesocket(sock);
	printf("Fermeture du serveur terminée\n");
        
	return EXIT_SUCCESS;
}
