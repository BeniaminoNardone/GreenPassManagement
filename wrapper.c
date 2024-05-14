#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "wrapper.h"
// La funzione Socket riceve come parametro la famiglia dell'indirizzo, il tipo di socket e il protocollo e
// restituisce un intero che rappresenta il descrittore del socket creato. La funzione gestisce gli eventuali errori,
// stampando un messaggio di errore e terminando il programma in caso di fallimento della chiamata di sistema.
int Socket(int family,int type,int protocol){
  int n;
  if((n = socket(family,type,protocol))<0){
    perror("socket");
    exit(1);
  }
  return n;
}
// La funzione Connect si occupa di stabilire una connessione su un socket creato precedentemente.
// Riceve come parametri il descrittore del socket, l'indirizzo del socket remoto e la dimensione dell'indirizzo.
// Gestisce gli eventuali errori, stampando un messaggio di errore e terminando il programma in caso di fallimento
// della chiamata di sistema.
int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen){
  if(connect(sockfd,addr,addrlen) < 0){
    perror("connect");
    exit(1);
  }
}
// La funzione Bind associa un indirizzo locale a un socket creato precedentemente. Riceve come parametri il
// descrittore del socket, l'indirizzo locale e la dimensione dell'indirizzo. Gestisce gli eventuali errori, stampando
// un messaggio di errore e terminando il programma in caso di fallimento della chiamata di sistema.
int Bind(int sockfd, const struct sockaddr *servaddr,socklen_t addrlen){
  if(bind(sockfd,servaddr, addrlen) < 0){
    perror("bind");
    exit(1);
  }
}
// La funzione Listen mette in ascolto il socket creato precedentemente, accettando eventuali connessioni in ingresso.
// Riceve come parametri il descrittore del socket e la lunghezza massima della coda di connessioni in attesa. Gestisce
// gli eventuali errori, stampando un messaggio di errore e terminando
// il programma in caso di fallimento della chiamata di sistema.
int Listen(int sockfd,int queue_lenght){
  if(listen(sockfd,queue_lenght) < 0){
    perror("listen");
    exit(1);
  }
}
// La funzione Accept accetta una connessione in ingresso su un socket creato precedentemente.
// Riceve come parametri il descrittore del socket, un puntatore all'indirizzo del socket remoto
// e un puntatore alla dimensione dell'indirizzo. Gestisce gli eventuali errori, stampando un messaggio
// di errore e terminando il programma in caso di fallimento della chiamata di sistema.
int Accept(int sockfd,struct sockaddr *restrict clientaddr,socklen_t *restrict addr_dim){
  int n;
  if((n = accept(sockfd,clientaddr, addr_dim))<0){
    perror("accept");
    exit(1);
  }
  return n;
}
