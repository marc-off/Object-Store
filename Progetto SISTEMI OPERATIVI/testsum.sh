#!/usr/bin/bash
	
	rm ./text/testout.txt
	kill -SIGUSR1 $(cat ./ObjectStore.pid)
	iconv -f UTF-8 -t ASCII//TRANSLIT ./text/testout.log>./text/testout.txt
	cat ./text/testout.txt

