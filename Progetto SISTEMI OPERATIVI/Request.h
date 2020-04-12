#if !defined(REQUEST_H)
#define REQUEST_H

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKNAME     "./objstore.sock"
#define MAXBACKLOG 32
/* Ho cercato di effettuare gestione di errori ed eventuali CONFORME POSIX il piú possibile! */

#define SYSCALL(r,c,e) \
    if((r=c)==-1) { perror(e);exit(errno); }
/* Gestione standard delle SYSCALL piu' comuni */
#define SYSCALLC(r,c,e) \
    if((r=c)==-1) { perror(e);return -1; }
/* Gestione standard delle SYSCALL piu' comuni a livello di funzionalita' client */
#define SYSCALLNOEXIT(r,c,e) \
    if((r=c)==-1) perror(e);
/* Gestione alternativa, segnalazione con perror ma NON USCIAMO */ 
#define CHECKNULL(r,c,e) \
    if ((r=c)==NULL) { perror(e); exit(errno); }
/* chiamate che restituiscono un valore */
#define CHECKNULLBREAK(r,c,e) \
    if ((r=c)==NULL) { perror(e); break;}    
/* chiamate void, principalmente threadRichiesta */
#define CHECKNULLNOEXIT(r,c,e) \
    if((r=c)==NULL) {perror(e);}
/* In chiamate void, segnalazione con perror + handler di protocollo + return */   
#define HANDLE1_PTHREAD(r, c, e, fd) \
    if ((r=c)!=0) { errno = s; perror(e); close(fd); return;}
/* Handler standard per le chiamate pthread */
#define HANDLE2_PTHREAD(r, c, e, ta, fd) \
    if ((r=c)!=0) {errno = s; perror(e); pthread_attr_destroy(&ta); close(fd); return;}  
/* Handler alternativo per le chiamate pthread, con distruzione del thread */
#define NOTUSED(expr) do { (void)(expr); } while (0)
/* Per ISOLARE gli avvertimenti del compilatore (principalmente i newline tokenizzati) */

char* tostring(long num)
{
    int i, rem, len = 0, n;
    char* str;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    str = (char*)malloc((len+1)*sizeof(char));
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';

return str;
}

char* register_request (char* name) {
/* Request: REGISTER name \n */
    char* buf=NULL;
    int n=strlen("REGISTER ")+strlen(name)+strlen(" \n")+1;
    /* determiniamo buf e quanto deve contenere */
    buf = realloc(buf, n*sizeof(char));
	if (!buf) return NULL;
    /* Allochiamo memoria */
    strcpy(buf, "REGISTER ");
    strcat(buf, name);
    strcat(buf, " \n");
    /* ricopiamo nel buffer REGISTER name \n */
    buf[n-1]='\0';
    /* NO BUFFER OVERFLOW */
    return buf;
}


char* store_request (char* name, char* block, int len)   {
/* Request: STORE name len \n data (in cui data è un blocco binario di lunghezza len bytes) */    

    char* buf = NULL;
    char* length = tostring(len);
    int n=strlen("STORE ")+strlen(name)+strlen(block)+strlen(" \n ")+strlen(length)+2;
    /* determiniamo buf e quanto deve contenere */
    buf=realloc(buf, n*sizeof(char));
    if (!buf) return NULL;
    /* Allochiamo memoria */
    strcpy(buf, "STORE ");
    strcat(buf, name);
    strcat(buf, " ");
    strcat(buf, length) ;
    strcat(buf, " \n ");
    strcat(buf, block);
    /* ricopiamo nel buffer STORE name len \n data*/
    buf[n-1]='\0';
    /* NO BUFFER OVERFLOW */
    free(length);
    return buf;
}


char* retrieve_request (char*name)  {
/* Request: RETRIEVE name \n */    
    char* buf=NULL;
    int n=strlen("RETRIEVE ")+strlen(name)+strlen(" \n")+1;
    /* determiniamo buf e quanto deve contenere */
    buf = realloc(buf, n*sizeof(char));
	if (!buf) return NULL;
    /* Allochiamo memoria */
    strcpy(buf, "RETRIEVE ");
    strcat(buf, name);
    strcat(buf, " \n");
    /* ricopiamo nel buffer RETRIEVE name \n */
    buf[n-1]='\0';
    /* NO BUFFER OVERFLOW */
    return buf;
}


char* delete_request (char*name)  {
/* Request: DELETE name \n */
    char* buf=NULL;
    int n=strlen("DELETE ")+strlen(name)+strlen(" \n")+1;
    /* determiniamo buf e quanto deve contenere */
    buf = realloc(buf, n*sizeof(char));
	if (!buf) return NULL;
    /* Allochiamo memoria */
    strcpy(buf, "DELETE ");
    strcat(buf, name);
    strcat(buf, " \n");
    /* ricopiamo nel buffer DELETE name \n */
    buf[n-1]='\0';
    /* NO BUFFER OVERFLOW */
    return buf;
}

char* leave_request()  {
/* Request: LEAVE \n */
    char* buf=NULL;
    int n=strlen("LEAVE")+strlen(" \n")+1;
    /* determiniamo buf e quanto deve contenere */
    buf = realloc(buf, n*sizeof(char));
    if (!buf) return NULL;
    /* Allochiamo memoria */
    strcpy(buf, "LEAVE");
    strcat(buf, " \n");
    /* ricopiamo nel buffer LEAVE \n */
    buf[n-1]='\0';
    /* NO BUFFER OVERFLOW */
    return buf;
}


char* response_format (int state, char* name)  {
/* Response = OK oppure KO (con conseguente messaggio)*/
    char* buf=NULL;
    int n=3*sizeof(char)+strlen(name)+strlen(" \n")+1;
    /* determiniamo buf e quanto deve contenere */
    buf = realloc(buf, n*sizeof(char));
    if (!buf) return NULL;
    /* Allochiamo memoria */
    if (state == 0) 
        strcpy(buf, "OK ");
    else 
        strcpy(buf, "KO ");
    /* In base allo stato determino la risposta: OK oppure KO */
    strcat(buf, name);
    strcat(buf, " \n");
    /* ricopiamo nel buffer OK/KO message \n */
    buf[n-1]='\0';
    /* NO BUFFER OVERFLOW */
    return buf;
}


int* codifytoascii (char* name) {
    int* ascii=NULL;
    int n = strlen(name)+1;
    /* Creiamo un array di valori ascii */
    ascii=realloc(ascii, n*sizeof(int));
    /* Allochiamo memoria */
    if (!ascii) {
        free(name); /* non ho piú memoria, meglio cominciare a liberare spazio..! */
        return NULL;
    }
    /* Allocazione FALLITA! */
    int i;
    for (i=0; i<n; i++)   {
        ascii[i] = (int)name[i];
    }
    ascii[n-1]=0; /* supponendo che 0 sia l'equivalente del carattere '\0' */
    /* Conversione di ogni carattere in valore ASCII */
    free(name);
    /* A questo punto, name non ci serve piu'! */
    return ascii;
}    


char* decodifyfromascii (int* ascii, int n_elem)    {
    char* decodify=NULL;
    int n = n_elem;  /* decodificando, abbiamo giá incluso il carattere di fine stringa '\0' */
    /* Creiamo una stringa per ogni valore ascii (COMPRESO '\0') */
    decodify=realloc(decodify, n*sizeof(int));
    /* Allochiamo memoria */
    if (!decodify) {
        free(ascii); /* non ho piú memoria, meglio cominciare a liberare spazio..! */
        return NULL;
    }
    /* Allocazione FALLITA! */
    int i;
    for (i=0; i<n; i++)   {
        decodify[i] = (char)ascii[i];
    }
    /* Conversione di ogni valore ASCII in carattere */
    free(ascii); /* una volta decodificata la richiesta, non ne abbiamo piú bisogno comunque..! */
    return decodify;
}


static inline int readn(long fd, void *buf, size_t size) {
/* Funzione che legge in un file descriptor FD da un buffer BUF a blocchi di size_t SIZE */
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) { /* Finché abbiamo dello spazio da cui leggere */
	if ((r=read((int)fd ,bufptr,left)) == -1) { /* OK Lettura */
	    if (errno == EINTR) continue;   /* Interrupted System Call */
	    return -1;
	}
	if (r == 0) return 0;   /* END OF FILE */
        left    -= r;
	    bufptr  += r;
    }
    return size;
}

static inline int writen(long fd, void *buf, size_t size) {
/* Funzione che scrive da un file descriptor FD in un buffer BUF a blocchi di size_t SIZE */    
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) { /* Finché abbiamo dello spazio da cui leggere */
	if ((r=write((int)fd ,bufptr,left)) == -1) {    /* OK Scrittura */
	    if (errno == EINTR) continue;
	    return -1;
	}
	if (r == 0) return 0;  /* END OF FILE */
        left    -= r;      
	bufptr  += r;
    }
    return 1;
}

#endif /* REQUEST_H */
