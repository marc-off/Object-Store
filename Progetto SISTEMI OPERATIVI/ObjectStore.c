#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "Request.h"

static pthread_mutex_t mtx = \
PTHREAD_MUTEX_INITIALIZER;

/* "..Quando il server riceve il segnale SIGUSR1,
vengono stampate sullo standard output alcune informazioni di stato del server; tra queste, almeno le
seguenti: numero di client connessi, numero di oggetti nello store, size totale dello store." */


static unsigned int numero_clienti_collegati = 0;

static unsigned int numero_oggetti_os = 0;

static unsigned long size_os = 0;

void gestore (int sig) {
	write(1, "SIGUSR1 catturato\n", 18);
	
	write(1, "Numero di clienti connessi  ", 30);
	char* clients=tostring(numero_clienti_collegati);
	write(1, clients, strlen(clients));
	free(clients);

	write(1, "\nNumero di oggetti memorizzati  ", 35);
	char* objects=tostring(numero_oggetti_os);
	write(1, objects, strlen(objects));
	free(objects);

	write(1, "\nSize attuale dell'object store  ", 36);
	char* size=tostring(size_os);
	write(1, size, strlen(size));
	free(size);

	write(1, "\n", 1);
}

// versione 1 thread per connessione (thread lanciati in modalita' detached)
void cleanup() {
    unlink(SOCKNAME);
}



void handle_response(int esito, int sfd, char* message)    {
/* Gestione standard formato OK/KO "message" */
    int notused;
    char* response;
    if (esito == 0)
        response = response_format (0, message);
    else 
        response = response_format (-1, message);
    /* Determiniamo il formato del response in base all' esito : (0) = OK , (!=0, di solito -1) = KO */
    int response_length = strlen(response)+1;
    SYSCALL(notused, writen(sfd, &response_length, sizeof(int)), "KO response 1 al cliente");
    /* Inviamo il numero di elementi che dovra' contenere il buffer del client per ricevere il responso */
    SYSCALL(notused, writen(sfd, response, response_length*sizeof(char)), "KO response 2 al cliente");
    /* Inviamo il responso stesso determinato inizialmente con la chiamata "response_format" */
    free(response);
    /* Ottimizziamo la gestione della memoria */
}



void handle_response_retrieve(int sfd, char* data, int len)    {
/* Gestione specifica per la retrieve formato DATA len \n data oppure KO "message" */
    int notused;
    char* lens = tostring(len);
    int response_length = strlen("DATA ")+strlen(lens)+strlen(" \n")+strlen(data)+2;

    char* response=NULL;
    /* determiniamo buf e quanto deve contenere */
    response = realloc(response, response_length*sizeof(char));
    if (!response) 
    {
        perror("Memoria esaurita! (handle_response_retrieve)");
        return;
    }
    /* Allochiamo memoria */
    strcpy(response, "DATA ");
    strcat(response, lens);
    strcat(response, " \n");
    strcat(response, data);
    /* ricopiamo nel buffer DATA len \n data */
    response[response_length-2]='\n';
    response[response_length-1]='\0';
    /* NO BUFFER OVERFLOW */
    SYSCALL(notused, writen(sfd, &response_length, sizeof(int)), "KO response 1 al cliente");
    /* Inviamo il numero di elementi che dovra' contenere il buffer del client per ricevere il responso */
    SYSCALL(notused, writen(sfd, response, response_length*sizeof(char)), "KO response 2 al cliente");
    /* Inviamo il responso stesso determinato inizialmente con la chiamata "response_format" */
    free(lens);
    free(response);
    /* Ottimizziamo la gestione della memoria */
}

/* Creazione spazio privato per il cliente "name" nell' object store (se gia' esiste RETURN PATHNAME DIRECTORY) */
char* createprivateOS(char *name, int sfd) {
    char* directory=NULL;
    int dirlength=strlen(name)+8; // "./data/" 7 caratteri, + '\0' sono 8
    CHECKNULLNOEXIT(directory, realloc(directory, dirlength*sizeof(char)), "KO, creazione directory (realloc)")
    if (directory == NULL)      
    {    
        handle_response(-1, sfd, "creazione directory FALLITA!\n");
        return directory;
    }
    /* Allocazione memoria con gestione degli errori e del protocollo */
    strcpy(directory, "./data/");
    strcat(directory, name);
    directory[dirlength-1]='\0';
    /* Directory predefinita per le cartelle cliente */
    struct stat st = {0};
    /* Struct che ci serve per determinare l'esistenza o meno della directory */
    if (stat(directory, &st) == -1)     
    {
	mkdir(directory, 0777);      
	numero_clienti_collegati++;
    }
    /* Caso 1 : non esiste, ne creiamo una nuova */
    handle_response(0, sfd, "registrazione avvenuta con SUCCESSO!\n");
    /* Gestione del protocollo di risposta */
    return directory;
    /* Salviamo la directory nel thread delle richieste, per future store/retrieve/delete */
}


/*  */
void storeobject (char *name, char *block, char* len, char* directory, int sfd)   {
    
    struct sigaction s;
    char* pathname=NULL;    
    int notused, path_length=strlen(directory)+strlen(name)+2;
    CHECKNULLNOEXIT(pathname, realloc(pathname, path_length*sizeof(char)), "KO, creazione file (realloc)")
    if (pathname == NULL)      
    {    
        handle_response(-1, sfd, "creazione file FALLITA!\n");
        return;
    }
    /* Allocazione memoria con gestione degli errori e del protocollo di risposta */
    strcpy(pathname, directory);
    strcat(pathname, "/");
    strcat(pathname, name);
    pathname[path_length-1]='\0';
    /* Directory privata del cliente nell'object store */
    SYSCALL(notused, sigaction(SIGUSR1,NULL,&s), "Gestione SEGNALE SIGUSR1 (1)");
    s.sa_handler=gestore;
    SYSCALL(notused, sigaction(SIGUSR1,&s,NULL), "Gestione SEGNALE SIGUSR1 (2)");
    int objfile = open(pathname, O_RDWR|O_CREAT|O_APPEND, 0777);
    /* Creazione del file che conterra' la stringa di testo prefissata char* block */
    int length = strtol(len, NULL, 10);
    /* Convertiamo dal buffer il numero (int) di lunghezza in bytes dell'oggetto block */
    while (((notused=writen(objfile, block , length)) == -1) && (errno==EINTR))
    ;
    /* Scrittura nel file objfile del blocco block per una lunghezza di length bytes */
    if (notused < 0)
    {  
        perror("KO, scrittura nel file (writen)");
	handle_response(-1, sfd, "scrittura nel file FALLITA!\n");
	unlink(pathname);
    }
    else 
    {    
    	handle_response(0, sfd, "store avvenuta con SUCCESSO!\n");
	numero_oggetti_os++;
	size_os = size_os+length;
    	close (objfile);  
    }
        /* Gestione del protocollo di risposta */

free(pathname);
return;
}


void retrieveobject (char *objname, char* directory, int sfd)    {
    char* pathname=NULL;
    int path_length=strlen(directory)+strlen(objname)+2;
    CHECKNULLNOEXIT(pathname, realloc(pathname, path_length*sizeof(char)), "KO, recupero file (realloc1)")
    if (pathname == NULL)      
    {    
        handle_response(-1, sfd, "recupero file FALLITO!\n");
        return;
    }
    /* Allocazione memoria con gestione degli errori e del protocollo */
    strcpy(pathname, directory);
    strcat(pathname, "/");
    strcat(pathname, objname);
    pathname[path_length-1]='\0';
    /* Pathname dell'oggetto contenuta nella directory */
    int fd, lung, notused;
    SYSCALLNOEXIT(fd, open(pathname, O_RDONLY), "KO, recupero file (open)");
    if (fd == -1)
    {
        handle_response(-1, sfd, "recupero file FALLITO!\n");
        free(pathname);
        return;
    }
    /* Apertura del file oggetto */    
    struct stat buffer;
    int         status;
    char* buf=NULL;
    status = stat(pathname, &buffer);
    if(status == 0)
    {
        CHECKNULLNOEXIT(buf, realloc(buf, (buffer.st_size)*sizeof(char)), "KO, recupero file (realloc2)");
        if (buf == NULL)
        {
            handle_response(-1, sfd, "recupero file FALLITO!\n");
            free(pathname);
            return;
        }    
        /* Allocazione memoria con gestione degli errori e del protocollo */
    }
    /* Determiniamo size del file */
    SYSCALLNOEXIT(lung, readn(fd, buf, buffer.st_size), "KO, lettura da file (readn)")
    if ( lung == -1) 
        { 
            handle_response(-1, sfd, "recupero file FALLITO!\n");
            free(pathname);
            return;
        }
    /* Recupero del file con eventuale gestione degli errori */
    SYSCALLNOEXIT(notused, close(fd), "KO, chiusura file oggetto");
    if (notused == -1)
    {
        free(buf);
        return;
    }
    /* Chiusura del file */
    handle_response_retrieve(sfd, buf, (int)buffer.st_size);

free(buf);
free(pathname);
return;
}



void deleteobject (char *objname, char* directory, int sfd)  {
    char* pathname=NULL;
    int notused, path_length=strlen(directory)+strlen(objname)+2;
    CHECKNULLNOEXIT(pathname, realloc(pathname, path_length*sizeof(char)), "KO, cancellazione file (realloc)")
    if (pathname == NULL)      
    {    
        handle_response(-1, sfd, "cancellazione file FALLITA!\n");
        return;
    }
    /* Allocazione memoria con gestione degli errori e del protocollo */
    strcpy(pathname, directory);
    strcat(pathname, "/");
    strcat(pathname, objname);
    pathname[path_length-1]='\0';
    /* Formulazione del pathname dell' oggetto da cancellare */
    struct stat buffer;
    notused = stat(pathname, &buffer);
    SYSCALLNOEXIT(notused, unlink(pathname), "KO, cancellazione file (unlink)");
    if (notused == -1)  
        handle_response(-1, sfd, "recupero file FALLITA!\n");
    else 
    {        
	handle_response(0, sfd, "cancellazione file avvenuta con SUCCESSO!\n");
	size_os = size_os-buffer.st_size;    	
    }
free(pathname);
return;
}


void *threadRichiesta(void *arg) {
    assert(arg);
    long connfd = (long)arg;
    char* client_directory=NULL;
    pthread_mutex_lock(&mtx);
    do {
//  PASSO 1 : RICEZIONE DELLA RICHIESTA

    int notused, nbuf;
    SYSCALLNOEXIT(notused, readn(connfd, &nbuf, sizeof(int)),"KO, (read1, nbuf)");
    if (notused == -1) 
    {
        handle_response(-1, connfd, "connessione FALLITA!\n");
        break;
    }
    /* Lettura dimensione buffer in arrivo */

    int* ascii_buf;     // leggiamo il buffer formato ASCII
    CHECKNULLNOEXIT(ascii_buf, (int*)malloc(nbuf*sizeof(int)), "KO, (malloc ascii_buf)");
    if (ascii_buf == NULL)
    {
        handle_response(-1, connfd, "connessione FALLITA!\n");
        break;
    }
    /* Allocazione memoria per il buffer di valori ASCII in arrivo */
    SYSCALLNOEXIT(notused, readn(connfd, ascii_buf, nbuf*sizeof(int)), "KO, (read2, ascii_buf)");
    if (notused == -1) 
    {
        handle_response(-1, connfd, "connessione FALLITA!\n");
        break;
    }
    /* Letura del buffer di valori ASCII */
    char* converted_buf;    // decodifichiamo da ASCII  
    CHECKNULLNOEXIT(converted_buf, decodifyfromascii(ascii_buf, nbuf), "KO, connessione INTERROTTA! (conversione da ascii in stringa)"); 
    if (converted_buf == NULL)
    {
        handle_response(-1, connfd, "connessione FALLITA!\n");
        break;
    }
//  PASSO 2 : ELABORAZIONE DELLA RICHIESTA
    
    /* tokenizziamo la richiesta secondo il protocollo di comunicazione */
    
    if (strncmp(converted_buf,"REGISTER", 8*sizeof(char)) == 0)
    {   /* Request: REGISTER name \n */
        char* request=strtok(converted_buf, " ");
        NOTUSED(request);
        char* client_name=strtok(NULL, " ");
        client_directory= createprivateOS(client_name, connfd);
    }
    
    else if (strncmp(converted_buf, "STORE", 5*sizeof(char)) == 0)   
    {   /* Request: STORE name len \n data (in cui data è un blocco binario di lunghezza len bytes) */  
        char* request=strtok(converted_buf, " ");
        NOTUSED(request);
        char* name = strtok(NULL, " ");
        char* len = strtok(NULL, " ");
        char* newline = strtok(NULL, " ");
        NOTUSED(newline);
        char* block = strtok(NULL, "\0");
        storeobject (name, block, len, client_directory, connfd);
    }
    
    else if (strncmp(converted_buf, "RETRIEVE", 8*sizeof(char)) == 0)  
    {   /* Request: RETRIEVE name \n */  
        char* request=strtok(converted_buf, " ");
        NOTUSED(request);
        char* object_name=strtok(NULL, " ");
        retrieveobject(object_name, client_directory, connfd);
    }
    
    else if (strncmp(converted_buf, "DELETE", 6*sizeof(char)) == 0)
    {   /* Request: DELETE name \n */
        char* request=strtok(converted_buf, " ");
        NOTUSED(request);        
        char* object_name=strtok(NULL, " ");
        deleteobject(object_name, client_directory, connfd);
    }
    
    else if (strncmp(converted_buf, "LEAVE", 5*sizeof(char)) == 0) 
    {   /* Request: LEAVE \n */
        char* request=strtok(converted_buf, " ");
        NOTUSED(request);
        free(converted_buf);
        handle_response(0, connfd, "disconnessione...\n");
        break;
    }
    
//    else
//    {
//        perror("KO, formato della richiesta NON identificata");
//        break;
//    }   
    
    free(converted_buf);
    } while (1);
    pthread_mutex_unlock(&mtx);
    if (client_directory != NULL) free(client_directory);
    close(connfd);
    return NULL; 

}

void spawn_thread(long connfd) {
    pthread_attr_t thattr;
    pthread_t thid;
    int s;  // (0) SUCCESS, altrimenti (error value)

    /* AGGIORNATA IN VERSIONE POSIX + MACRO */
    
    HANDLE1_PTHREAD(s, pthread_attr_init(&thattr), "KO, connessione FALLITA! (pthread_attr_init)", 
        connfd);
/*  
    if (pthread_attr_init(&thattr) != 0) {
	fprintf(stderr, "KO, connessione FALLITA! (pthread_attr_init)\n");
	close(connfd);
	return;
    }   
*/
    // settiamo il thread in modalità detached
    HANDLE2_PTHREAD(s, pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED), 
        "KO, connessione FALLITA! (pthread_attr_setdetachstate)", thattr, connfd);
/*  
    if (pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0) {
	fprintf(stderr, "KO, connessione FALLITA! (pthread_attr_setdetachstate)\n");
	pthread_attr_destroy(&thattr);
	close(connfd);
	return;
    }
*/    
    HANDLE2_PTHREAD(s, pthread_create(&thid, &thattr, threadRichiesta, (void*)connfd),
        "KO, connessione FALLITA! (pthread_create)", thattr, connfd);
/*
    if (pthread_create(&thid, &thattr, threadRichiesteCliente, (void*)connfd) != 0) {
	fprintf(stderr, "KO, connessione FALLITA! (pthread_create)\n");
	pthread_attr_destroy(&thattr);
	close(connfd);
	return;
    }
*/
}


int main(int argc, char *argv[]) {
    cleanup();    
    atexit(cleanup);    

    int listenfd;
    SYSCALL(listenfd, socket(AF_UNIX, SOCK_STREAM, 0), "(S) Creazione socket");

    struct sockaddr_un serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;    
    strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME)+1);

    int notused;
    SYSCALL(notused, bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)), "(S) Bind socket");
    SYSCALL(notused, listen(listenfd, MAXBACKLOG), "(S) Listen connessioni da parte dei client");


    while(1) {      
	long connfd;
	SYSCALL(connfd, accept(listenfd, (struct sockaddr*)NULL ,NULL), 
        "(S) Accept richiesta di connessione del client");
	//printf("connection accepted\n");    	
	spawn_thread(connfd);
    }
    return 0;
}
