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
Questo è il punto di ingresso del programma, la funzione "main". Prende due argomenti: il numero di argomenti passati dalla riga di comando (argc) e un array di stringhe che contiene gli argomenti (argv).
*/
int main(int argc,char **argv) {
//Viene dichiarato un descrittore di socket e una struttura di tipo "sockaddr_in" contenente le informazioni sull'indirizzo del server.
  int sockfd,n;
  struct sockaddr_in servaddr;

//Viene verificato che siano stati forniti esattamente due argomenti sulla riga di comando, ovvero l'indirizzo IP del server e il numero di tessera sanitaria. In caso contrario, il programma emette un messaggio di errore e termina l'esecuzione.
  if(argc != 3){
    fprintf(stderr,"usage: %s <IPaddress> <TesseraSanitaria>\n",argv[0]);
    exit(1);
  }

//Viene verificato che il secondo argomento (il numero di tessera sanitaria) sia lungo esattamente 20 caratteri. In caso contrario, il programma emette un messaggio di errore e termina l'esecuzione.
  if(strlen(argv[2]) != 20){
    fprintf(stderr,"La tessera sanitaria fornita non risulta essere valida. \n");
    exit(1);
  }

//Viene creato un socket utilizzando la funzione "Socket" definita nella libreria "wrapper.h". La funzione prende tre argomenti: l'indirizzo della famiglia di protocolli (AF_INET per IPv4), il tipo di socket (SOCK_STREAM per una connessione TCP) e il protocollo (0 per il protocollo di default).
  sockfd = Socket(AF_INET,SOCK_STREAM,0);

//La struttura "sockaddr_in" viene inizializzata con l'indirizzo della famiglia di protocolli (AF_INET per IPv4) e la porta a cui il server è in ascolto (la porta 1024).
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(1024);


//converte l'indirizzo IP passato come primo argomento da stringa a formato binario e lo assegna al campo sin_addr della struttura servaddr. Se la conversione fallisce, stampa un messaggio di errore e termina il programma.
  if(inet_pton(AF_INET,argv[1], &servaddr.sin_addr) < 0) {
    fprintf(stderr,"inet_pton error for %s\n",argv[1]);
    exit(1);
  }

  //connette il socket identificato da sockfd all'indirizzo IP e alla porta specificati nella struttura servaddr.
  Connect(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr));

  //invia al socket i dati contenuti nella stringa passata come secondo argomento e ne verifica l'avvenuto invio. Se l'invio fallisce, stampa un messaggio di errore e termina il programma.
if( write(sockfd,argv[2],strlen(argv[2])) != strlen(argv[2])) {
  perror("write");
  exit(1);
}
printf("La tessera sanitaria è stata consegnata con successo. \n");
//chiude il socket.
  close(sockfd);
}
