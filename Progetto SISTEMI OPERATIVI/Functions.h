#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

int os_connect (char* name)	{	
//	PASSO 1 : CONNESSIONE AL SERVER

	struct sockaddr_un serv_addr;
    /* Struct per la connessione all'indirizzo del server */
	int client_socket;
    SYSCALLC(client_socket, socket(AF_UNIX, SOCK_STREAM, 0), "Creazione socket");
    memset(&serv_addr, '0', sizeof(serv_addr));
	/* Creazione socket client */
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path,SOCKNAME, strlen(SOCKNAME)+1);
	/* Copio SOCKNAME nella struct */
    int notused;
    SYSCALLC(notused, connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)), "Connessione al server");
    /* Connessione al server */

//	PASSO 2 : GESTIONE PROTOCOLLO (REQUEST e RESPONSE)

	char *request;
	CHECKNULL(request, register_request(name), "REGISTER memoria esaurita 1");
	/* Formuliamo la richiesta di registrazione - Allocazione memoria e gestione errore */
	int request_length = strlen(request)+1;	
	SYSCALLC(notused, writen(2, request, request_length*sizeof(char)), "REGISTER (C) stampa su stderr 1");
	/* Stampa della richiesta su terminale del cliente */
	int* ascii_request;
	CHECKNULL(ascii_request, codifytoascii(request), "REGISTER memoria esaurita 2");
	/* Conversione della richiesta in ASCII - Allocazione memoria e gestione errore*/
	SYSCALLC(notused, writen(client_socket, &request_length, sizeof(int)), "REGISTER write 1");
	SYSCALLC(notused, writen(client_socket, ascii_request, request_length*sizeof(int)), "REGISTER write 2");
    /* Formulo due richieste di write, numero di elementi e array di ascii */
	free(ascii_request);
	
	int n, state;
	char* response;
    SYSCALLC(notused, readn(client_socket, &n, sizeof(int)), "RESPONSO (C) read 1");
    CHECKNULL(response, malloc(n*sizeof(char)), "RESPONSO (C) memoria esaurita 3");
    /* Allocazione memoria per il buffer di read */
	SYSCALLC(notused, readn(client_socket, response, n*sizeof(char)), "RESPONSO (C) read 2");
	response[n-1] = '\0';
	/* Formulo due richieste di read, numero di elementi e buffer di responso */
	SYSCALLC(notused, writen(2, response, n), "RESPONSO (C) stampa su stderr 2");
	/* Stampa sul terminale del cliente la risposta del server */
	char* esito = strtok(response, " ");
	if (strcmp(esito, "OK") == 0)	state = client_socket;
	if (strcmp(esito, "KO") == 0)	state = -1;

	if (strcmp(esito, "OK") != 0 && strcmp(esito, "KO") != 0)	
	{
		perror("elaborazione responso FALLITA!");
		state = -1;
	}

free(response);
return state;	/* Return true or false in base alla risposta del server */
}




int os_store (char *name, char *block, int len, int cfd)	{
// 	Request : STORE name len \n data

// 	GESTIONE PROTOCOLLO (REQUEST E RESPONSE)

	int notused;
	char *request;
	CHECKNULL(request, store_request(name, block, len), "STORE memoria esaurita 1\n");
	/* Formuliamo la richiesta di memorizzazione - Allocazione memoria e gestione errore */
	int request_length = strlen(request)+1;
	SYSCALLC(notused, writen(2, request, request_length*sizeof(char)), "STORE(C) stampa su stderr\n");
	/* Stampa della richiesta su terminale del cliente */
	int* ascii_request;
	CHECKNULL(ascii_request, codifytoascii(request), "STORE memoria esaurita 2\n");
	/* Conversione della richiesta in ASCII - Allocazione memoria e gestione errore*/
	SYSCALLC(notused, writen(cfd, &request_length, sizeof(int)), "STORE write 1\n");
	SYSCALLC(notused, writen(cfd, ascii_request, request_length*sizeof(int)), "STORE write 2\n");
	/* Formulo due richieste di write, numero di elementi e array di ascii */
	free(ascii_request);
    
    int n, state;
    char* response;
    SYSCALLC(notused, readn(cfd, &n, sizeof(int)), "REGISTER read 1");
    CHECKNULL(response, malloc(n*sizeof(char)), "REGISTER memoria esaurita 3");
    /* Allocazione memoria per il buffer di read */
	SYSCALLC(notused, readn(cfd, response, n*sizeof(char)), "REGISTER read 2");
	response[n-1] = '\0';
	/* Formulo due richieste di read, numero di elementi e buffer di responso */
	SYSCALLC(notused, writen(2, response, n), "REGISTER (C) stampa su stderr 2");
	/* Stampa sul terminale del cliente la risposta del server */
	char* esito = strtok(response, " ");
	if (strcmp(esito, "OK") == 0)	state = cfd;
	if (strcmp(esito, "KO") == 0)	state = -1;

	if (strcmp(esito, "OK") != 0 && strcmp(esito, "KO") != 0)	
	{
		perror("elaborazione responso FALLITA!");
		state = -1;
	}
free(response);
return state;	/* Return true or false in base alla risposta del server */
}


int os_retrieve (char *name, int cfd)	{
//	Request: RETRIEVE name \n
	
// 	GESTIONE PROTOCOLLO (REQUEST E RESPONSE)
	int notused;
	char *request;
	CHECKNULL(request, retrieve_request(name), "STORE memoria esaurita 1\n");
	/* Formuliamo la richiesta di memorizzazione - Allocazione memoria e gestione errore */
	int request_length = strlen(request)+1;
	SYSCALLC(notused, writen(2, request, request_length*sizeof(char)), "STORE(C) stampa su stderr\n");
	/* Stampa della richiesta su terminale del cliente */
	int* ascii_request;
	CHECKNULL(ascii_request, codifytoascii(request), "STORE memoria esaurita 2\n");
	/* Conversione della richiesta in ASCII - Allocazione memoria e gestione errore*/
	SYSCALLC(notused, writen(cfd, &request_length, sizeof(int)), "STORE write 1\n");
	SYSCALLC(notused, writen(cfd, ascii_request, request_length*sizeof(int)), "STORE write 2\n");
	/* Formulo due richieste di write, numero di elementi e array di ascii */
	free(ascii_request);    

    int n, state;
    char* response;
    SYSCALLC(notused, readn(cfd, &n, sizeof(int)), "REGISTER read 1");
    CHECKNULL(response, malloc(n*sizeof(char)), "REGISTER memoria esaurita 3");
    /* Allocazione memoria per il buffer di read */
	SYSCALLC(notused, readn(cfd, response, n*sizeof(char)), "REGISTER read 2");
	response[n-1] = '\0';
	/* Formulo due richieste di read, numero di elementi e buffer di responso */
	SYSCALLC(notused, writen(2, response, n), "REGISTER (C) stampa su stderr 2");
	char* esito = strtok(response, " ");
	if (strcmp(esito, "DATA") == 0)	state = cfd;
	if (strcmp(esito, "KO") == 0)	state = -1;

	if (strcmp(esito, "DATA") != 0 && strcmp(esito, "KO") != 0)	
	{
		perror("elaborazione responso FALLITA!");
		state = -1;
	}
free(response);
return state;	
}



int os_delete (char *name, int cfd)	{
//	Request: DELETE name \n

// 	GESTIONE PROTOCOLLO (REQUEST E RESPONSE)
	int notused;
	char *request;
	CHECKNULL(request, delete_request(name), "DELETE memoria esaurita 1");
	/* Formuliamo la richiesta di cancellazione - Allocazione memoria e gestione errore */
	int request_length = strlen(request)+1;	
	SYSCALLC(notused, writen(2, request, request_length*sizeof(char)), "DELETE (C) stampa su stderr 1");
	/* Stampa della richiesta su terminale del cliente */
	int* ascii_request;
	CHECKNULL(ascii_request, codifytoascii(request), "DELETE memoria esaurita 2");
	/* Conversione della richiesta in ASCII - Allocazione memoria e gestione errore*/
	SYSCALLC(notused, writen(cfd, &request_length, sizeof(int)), "DELETE write 1");
	SYSCALLC(notused, writen(cfd, ascii_request, request_length*sizeof(int)), "DELETE write 2");
    /* Formulo due richieste di write, numero di elementi e array di ascii */
	free(ascii_request);	

	int n, state;
	char* response;
    SYSCALLC(notused, readn(cfd, &n, sizeof(int)), "RESPONSO (C) read 1");
    CHECKNULL(response, malloc(n*sizeof(char)), "RESPONSO (C) memoria esaurita 3");
    /* Allocazione memoria per il buffer di read */
	SYSCALLC(notused, readn(cfd, response, n*sizeof(char)), "RESPONSO (C) read 2");
	response[n-1] = '\0';
	/* Formulo due richieste di read, numero di elementi e buffer di responso */
	SYSCALLC(notused, writen(2, response, n), "RESPONSO (C) stampa su stderr 2");
	/* Stampa sul terminale del cliente la risposta del server */
	char* esito = strtok(response, " ");
	if (strcmp(esito, "OK") == 0)	state = cfd;
	if (strcmp(esito, "KO") == 0)	state = -1;

	if (strcmp(esito, "OK") != 0 && strcmp(esito, "KO") != 0)	
	{
		perror("elaborazione responso FALLITA!");
		state = -1;
	}

free(response);	
return state;
/* Return true or false in base alla risposta del server */
}




int os_disconnect (int cfd)	{
//	Request: LEAVE name \n

// 	GESTIONE PROTOCOLLO (REQUEST E RESPONSE)
	int notused;
	char *request;
	CHECKNULL(request, leave_request(), "LEAVE memoria esaurita 1");
	/* Formuliamo la richiesta di cancellazione - Allocazione memoria e gestione errore */
	int request_length = strlen(request)+1;	
	SYSCALLC(notused, writen(2, request, request_length*sizeof(char)), "LEAVE (C) stampa su stderr 1");
	/* Stampa della richiesta su terminale del cliente */
	int* ascii_request;
	CHECKNULL(ascii_request, codifytoascii(request), "LEAVE memoria esaurita 2");
	/* Conversione della richiesta in ASCII - Allocazione memoria e gestione errore*/
	SYSCALLC(notused, writen(cfd, &request_length, sizeof(int)), "LEAVE write 1");
	SYSCALLC(notused, writen(cfd, ascii_request, request_length*sizeof(int)), "LEAVE write 2");
    /* Formulo due richieste di write, numero di elementi e array di ascii */
	free(ascii_request);

    int n, state;
	char* response;
    SYSCALLC(notused, readn(cfd, &n, sizeof(int)), "RESPONSO (C) read 1");
    CHECKNULL(response, malloc(n*sizeof(char)), "RESPONSO (C) memoria esaurita 3");
    /* Allocazione memoria per il buffer di read */
	SYSCALLC(notused, readn(cfd, response, n*sizeof(char)), "RESPONSO (C) read 2");
	response[n-1] = '\0';
	/* Formulo due richieste di read, numero di elementi e buffer di responso */
	SYSCALLC(notused, writen(2, response, n), "RESPONSO (C) stampa su stderr 2");
	/* Stampa sul terminale del cliente la risposta del server */
	char* esito = strtok(response, " ");
	if (strcmp(esito, "OK") == 0)	
		{
			close(cfd);
			state = 0;
		}
	else
	{
		perror("elaborazione responso FALLITA!");
		state = -1;
	}
free(response);
return state;
/* Return true or false in base alla risposta del server */
}

