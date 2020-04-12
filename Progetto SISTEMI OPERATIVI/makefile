# make file overview :
#	Il progetto dovrà includere un makefile avente, fra gli altri, i , clean (per ripulire la directory di lavoro dai file generati), e
#	test. Quest'ultimo deve eseguire un test dell'object store, lanciando dapprima in contemporanea 50 istanze del
#	client di test (ciascuna con nome diverso), che effettueranno test di tipo 1. Terminata l'esecuzione di queste
#	istanze, vanno lanciate – sempre in contemporanea – altre 50 istanze (con gli stessi nomi usati in
#	precedenza), di cui 30 devono eseguire test di tipo 2, e 20 test di tipo 3. L'output cumulato dei test, con
#	eventualmente altre informazioni utili, deve essere memorizzato in un solo file di nome testout.log
#	nella directory corrente.



# IMPOSTAZIONI PER COMPILAZIONE
cc= gcc -std=c99 
CFLAGS= -Wall -pedantic
SFLAGS= -Wall -pedantic -pthread
MAKE= make

# target all (per generare l'eseguibile dell'object store, la libreria, e l'eseguibile del client di test)

all : ClientTest ObjectStore
	$(MAKE) ClientTest
	$(MAKE) ObjectStore

ClientTest : ClientTest.o
	$(MAKE) ClientTest.o	
	$(cc) ClientTest.o -o ClientTest

ObjectStore : ObjectStore.o
	$(MAKE) ObjectStore.o	
	$(cc) -pthread ObjectStore.o -o ObjectStore

ClientTest.o : ClientTest.c Request.h Functions.h
	$(cc) $(CFLAGS) -c ClientTest.c

ObjectStore.o : ObjectStore.c Request.h
	$(cc) $(SFLAGS) -c ObjectStore.c

# target clean (per ripulire la directory di lavoro dai file generati)

clean : cleanobjstore.sh
	./cleanobjstore.sh

# target test : deve eseguire un test dell'object store, lanciando dapprima in contemporanea 50 istanze del
#	client di test (ciascuna con nome diverso), che effettueranno test di tipo 1. Terminata l'esecuzione di queste
#	istanze, vanno lanciate – sempre in contemporanea – altre 50 istanze (con gli stessi nomi usati in
#	precedenza), di cui 30 devono eseguire test di tipo 2, e 20 test di tipo 3. L'output cumulato dei test, con
#	eventualmente altre informazioni utili, deve essere memorizzato in un solo file di nome testout.log
#	nella directory corrente.

test : testobjstore.sh
	./testobjstore.sh





