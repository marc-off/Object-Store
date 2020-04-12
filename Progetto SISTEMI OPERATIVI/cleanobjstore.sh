#!/usr/bin/bash	

	echo "Rimuovo tutti i file oggetto e gli spazi cliente.."
	for CLIENTNAME in Augusto Marco Tito Tiberio Adriano Domiziano Nerone Claudio Vespasiano Caligola Otone Didio Antonino Commodo Traiano Caracalla Settimio Pertinace Vitellio Alessandro Nerva Galba Massimino Macrino Eliogabalo Geta Cesare Pompeo Tullio Gallio; do	
	rm -rf ./data/$CLIENTNAME
	done
	for CLIENTNAME in Saffo Sylvia Alda Emily Marina Wislawa Anne Percy Erinna Veronica Gaspara Agatha Maria Renee Elizabeth Anite Antonia Charlotte Vittoria Eleonora ; do
	rm -rf ./data/$CLIENTNAME
	done
	echo "Object Store pulito con SUCCESSO!"
