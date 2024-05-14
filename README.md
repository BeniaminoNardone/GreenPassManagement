# Descrizione del progetto
Il progetto ha come obiettivo la creazione di un servizio per la
gestione del Green Pass.
Il funzionamento del servizio prevede che un utente, dopo aver
effettuato la vaccinazione, utilizzi il ClientA per collegarsi ad un
centro vaccinale e comunicare il codice della propria tessera
sanitaria.
Il centro vaccinale, a sua volta, comunica al ServerV il codice
ricevuto dal client e il periodo di validità del Green Pass. Per
verificare la validità di un Green Pass, un ClientS invia il codice di
una tessera sanitaria al ServerG, il quale richiede al ServerV il
controllo della validità.
Inoltre, il ClientT può invalidare o ripristinare la validità di un Green
Pass comunicando al ServerG il contagio o la guarigione di una
persona attraverso il codice della tessera sanitaria. Infine, il ServerG
comunica con il ServerV per invalidare o ripristinare il Green Pass.

Link alla relazione completa: https://github.com/BeniaminoNardone/GreenPassManagement/blob/f5219aef93554ec280734104ed45e851099f43be/Relazione_Reti_Madonna_Mazza_Nardone.pdf
