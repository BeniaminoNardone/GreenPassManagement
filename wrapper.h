#ifndef _WRAPPER_H
#define _WRAPPER_H
#include <sys/socket.h>

/*
Questo codice definisce alcune funzioni di utilità per la gestione dei socket in C, inclusi:

Socket: crea un endpoint di comunicazione del socket e ritorna un descrittore di file per il socket.
Connect: stabilisce una connessione al socket remoto specificato dall'indirizzo e dalla porta dati in input.
Bind: associa un indirizzo locale a un socket.
Listen: mette in ascolto il socket per le connessioni in entrata.
Accept: accetta una connessione in arrivo e crea un nuovo socket con un descrittore di file separato per la comunicazione con il client.
*/

int Socket(int family,int type,int protocol);
int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int Bind(int sockfd, const struct sockaddr *servaddr,socklen_t addrlen);
int Listen(int sockfd,int queue_lenght);
int Accept(int sockfd,struct sockaddr *restrict clientaddr,socklen_t *restrict addr_dim);
#endif
