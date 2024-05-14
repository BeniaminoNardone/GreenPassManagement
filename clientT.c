#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "wrapper.h"
#include <time.h>

/*
Il codice definisce una struttura chiamata green_pass che contiene tre campi: una stringa di caratteri chiamata num_tesseraSanitaria con una lunghezza massima di 21 caratteri, un campo data_Scadenza di tipo time_t che rappresenta una data e un campo tipo_Servizio di tipo int.
La funzione main ha come parametri argc e argv che rappresentano rispettivamente il numero di argomenti passati alla linea di comando e un array di stringhe contenente gli argomenti stessi.
La funzione inizializza una variabile sockfd di tipo int che sarà usata in seguito per la connessione, una variabile servaddr di tipo sockaddr_in che conterrà l'indirizzo del server a cui ci si vuole connettere e una variabile greenPass di tipo green_pass.
Inoltre, il codice inizializza due array di caratteri: valid e invalid, entrambi di lunghezza 2.
*/
struct green_pass {
  char num_tesseraSanitaria[21];
  time_t data_Scadenza;
  int tipo_Servizio;
};

int main(int argc, char** argv){
  int sockfd,n;
  struct sockaddr_in servaddr;
  struct green_pass greenPass;
  char valid[] = "V";
  char invalid[] = "I";

//Il codice controlla se il numero di argomenti passati in input è diverso da 4. In caso affermativo, stampa un messaggio di errore e termina il programma. Il programma richiede che gli argomenti passati siano l'indirizzo IP del server, il numero di tessera sanitaria e una stringa che può essere "V" o "I".
  if(argc != 4){
    fprintf(stderr,"usage: %s <IPaddress> <TesseraSanitaria> <V or I>\n",argv[0]);
    exit(1);
  }

//Questo codice controlla se la lunghezza della stringa contenuta nell'argomento argv[2] è esattamente 20 caratteri. Se la lunghezza non è valida, viene stampato un messaggio di errore e il programma viene terminato con un codice di uscita di 1. 
  if(strlen(argv[2]) != 20){
    fprintf(stderr,"La tessera sanitaria fornita non risulta essere valida. \n");
    exit(1);
  }

/*
Questo codice legge il terzo argomento fornito all'esecuzione del programma e se è uguale alla stringa "V" assegna al campo tipo_Servizio della struttura greenPass il valore 1, altrimenti se è uguale alla stringa "I" assegna al campo tipo_Servizio il valore 2. Se l'argomento non è né "V" né "I" il programma stampa un messaggio di errore e termina.
Inoltre, il codice copia il valore del secondo argomento nella stringa tessera della struttura greenPass
*/
  strcpy(greenPass.num_tesseraSanitaria,argv[2]);

  if(strcmp(argv[3],valid) == 0){
  greenPass.tipo_Servizio = 1;
}else if(strcmp(argv[3],invalid) == 0){
  greenPass.tipo_Servizio = 2;
}else{
  printf("L'input (%s) utilizzato non è valido.",argv[3]);
  exit(1);
}

//Il codice crea un socket di tipo SOCK_STREAM usando l'indirizzo IPv4 (AF_INET) e il protocollo predefinito (0). Viene quindi impostata la famiglia di indirizzi (sin_family) e la porta (sin_port) nella struttura servaddr utilizzata per la connessione. In particolare, viene impostata la porta a 1026 utilizzando la funzione htons per la conversione del byte order.
sockfd = Socket(AF_INET,SOCK_STREAM,0);
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(1026);

/*
Questo codice converte l'indirizzo IP nella rappresentazione standard in formato testuale (come ad esempio "192.168.0.1") in un formato binario utilizzato dalla libreria di rete. Questo viene fatto utilizzando la funzione inet_pton della libreria arpa/inet.h.
Il primo parametro di inet_pton è la famiglia dell'indirizzo (AF_INET per gli indirizzi IPv4), il secondo parametro è l'indirizzo IP in formato testuale, mentre il terzo parametro è un puntatore all'indirizzo in formato binario in cui verrà memorizzato il risultato della conversione.
Se la conversione non riesce, la funzione restituisce un valore negativo e il programma termina con un messaggio di errore.
*/
if(inet_pton(AF_INET,argv[1], &servaddr.sin_addr) < 0) {
  fprintf(stderr,"inet_pton error for %s\n",argv[1]);
  exit(1);
}

/*
 Il codice utilizza la funzione Connect() per stabilire una connessione TCP tra il client e il server, passando come parametri il socket file descriptor (sockfd) e l'indirizzo del server (servaddr) con cui il client deve comunicare.
Successivamente, il codice utilizza la funzione write() per scrivere sul socket (sockfd) i dati contenuti nella variabile greenPass. La funzione write() restituisce il numero di byte scritti sul socket. Se il numero di byte scritti è diverso dalla dimensione dei dati contenuti in greenPass, viene stampato un messaggio di errore utilizzando la funzione perror() e il programma termina con un codice di uscita 1.
Infine, viene stampato un messaggio a video che indica che la richiesta è stata inoltrata correttamente utilizzando la funzione printf().
*/
Connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
if( write(sockfd,&greenPass,sizeof(greenPass)) != sizeof(greenPass)) {
perror("write");
exit(1);
}
printf("La richiesta è stata inoltrata con successo. \n");

/* il codice utilizza la funzione close() per chiudere il socket (sockfd) utilizzato per la comunicazione tra client e server. Chiudere il socket è necessario per rilasciare le risorse utilizzate e prevenire eventuali errori di comunicazione futuri.
Successivamente, il codice utilizza la funzione exit() per terminare il programma con un codice di uscita 0. Un codice di uscita 0 indica che il programma è stato terminato con successo. La funzione exit() viene utilizzata per terminare il programma in modo pulito e rilasciare le risorse del sistema utilizzate dal programma.
*/
close(sockfd);
exit(0);

}
