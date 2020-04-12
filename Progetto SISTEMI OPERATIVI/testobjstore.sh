#!/usr/bin/bash	
	rm ObjectStore.pid 	
	rm ./text/testout.log
#launch del server	
	./ObjectStore >> ./text/testout.log 2>&1 &
	echo $! >> ./ObjectStore.pid
#50 istanze di test di tipo 1
	for CLIENTNAME in Augusto Marco Tito Tiberio Adriano Domiziano Nerone Claudio Vespasiano Caligola Otone Didio Antonino Commodo Traiano Caracalla Settimio Pertinace Vitellio Alessandro Nerva Galba Massimino Macrino Eliogabalo Geta Cesare Pompeo Tullio Gallio Saffo Sylvia Alda Emily Marina Wislawa Anne Percy Erinna Veronica Gaspara Agatha Maria Renee Elizabeth Anite Antonia Charlotte Vittoria Eleonora; do
	./ClientTest $CLIENTNAME 1 >>./text/testout.log;
	done;
#30 istanze di test di tipo 2
	for CLIENTNAME in Augusto Marco Tito Tiberio Adriano Domiziano Nerone Claudio Vespasiano Caligola Otone Didio Antonino Commodo Traiano Caracalla Settimio Pertinace Vitellio Alessandro Nerva Galba Massimino Macrino Eliogabalo Geta Cesare Pompeo Tullio Gallio ; do
	./ClientTest $CLIENTNAME 2 >>./text/testout.log;
	done;
#20 istanze di test di tipo 3
	for CLIENTNAME in Saffo Sylvia Alda Emily Marina Wislawa Anne Percy Erinna Veronica Gaspara Agatha Maria Renee Elizabeth Anite Antonia Charlotte Vittoria Eleonora ; do
	./ClientTest $CLIENTNAME 3 >>./text/testout.log;
	done;
	
	
