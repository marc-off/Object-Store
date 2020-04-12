#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Request.h"
#include "Functions.h"

static unsigned int operazioni_effettuate=0;

static unsigned int operazioni_successo=0;

static unsigned int operazioni_fallite=0;

void stampa_esito(char* name)	{

	write(1, name, strlen(name));
	
	write(1, "\nNumero di operazioni effettuate ", 32);
	char* done=tostring(operazioni_effettuate);
	write(1, done, strlen(done));
	free(done);

	write(1, "\nNumero di operazioni con successo ", 34);
	char* success=tostring(operazioni_successo);
	write(1, success, strlen(success));
	free(success);

	write(1, "\nNumero di operazioni fallite ", 30);
	char* failed=tostring(operazioni_fallite);
	write(1, failed, strlen(failed));
	free(failed);

	write(1, "\n", 1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {	/* Assumiamo anche che il numero di test sia compreso tra 1 e 3 */
		perror("Formula una richiesta del tipo: [nome dell'eseguibile] [nome del client]\n");
		exit(EXIT_FAILURE);
    }

	/* Richiesta di registrazione da parte del Client nell'Object Store */
	int client_socket, esito;  /* neff=0, nsucc=0, nfail=0 */
	if ((client_socket=os_connect(argv[1])) < 0)	
	{			
		operazioni_effettuate++; operazioni_fallite++;
		stampa_esito(argv[1]);
		return -1;
	}
	else	{ operazioni_effettuate++; operazioni_successo++; }

	/* Codifica della batteria di test */
	long choice = strtol(argv[2], NULL, 10);

	if (choice == 1)
	{		int length = 100;
			while ((length<=100000)&&(esito!=1))
			/* generiamo 20 oggetti di dimensioni crescenti
				da 100 a 100.000 bytes */	
			{
				int plusize = strlen("Pluto")*sizeof(char);
				/* 6 caratteri * 1 byte */
				int seqlen = plusize;
				int n = (length/sizeof(char))+1;
				char* seq = (char*)malloc(length+1);
				strcpy(seq, "Pluto");
				int left_chars = length-5;
				/* Creiamo un oggetto (char*) di length bytes */
				for (seqlen=plusize; seqlen<=length; seqlen=seqlen+plusize)	
				{	
					strncat(seq, "Pluto", left_chars*sizeof(char));
					left_chars = left_chars - 5;
				}
				/* Copiamo una stringa di testo prefissata */
				seq[n-2]='\n';
				seq[n-1]='\0';
				/* NO BUFFER OVERFLOW */
				esito = os_store ("Pluto", seq, length, client_socket);
				if (esito<0)	operazioni_fallite++;
				else 	operazioni_successo++;
				operazioni_effettuate++;
				free(seq);

			length=length+5257;
			}
	}
		
	if (choice == 2)
	{
			esito = os_retrieve ("Pluto", client_socket);
			if (esito<0)	operazioni_fallite++;
			else 	operazioni_successo++;
			operazioni_effettuate++;
	}

	if (choice == 3)
	{
			esito = os_delete ("Pluto", client_socket);
			if (esito<0)	operazioni_fallite++;
			else 	operazioni_successo++;
			operazioni_effettuate++;
	}

	esito=os_disconnect(client_socket);
	if (esito<0)	operazioni_fallite++;
	else 	operazioni_successo++;
	operazioni_effettuate++;


stampa_esito(argv[1]);
if (esito<0) return -1;
return 0;
}

