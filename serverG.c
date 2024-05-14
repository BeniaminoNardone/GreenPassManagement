#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include "wrapper.h"
#include <time.h>



//Questo codice dichiara una struttura chiamata "green_pass" che contiene tre campi: una stringa chiamata "num_tesseraSanitaria" lunga 21 caratteri, un valore "data_Scadenza" di tipo time_t e un intero "tipo_Servizio".

struct green_pass {
  char num_tesseraSanitaria[21];
  time_t data_Scadenza;
  int tipo_Servizio;
};

/*
Il codice dichiara la funzione "main" con tre argomenti: un intero "argc" e un array di stringhe "argv". Nella funzione "main", vengono dichiarate diverse variabili: un intero "sockfd", un intero "listenfd", un intero "n", un intero "requestfd", un'istanza della struttura "green_pass" e due istanze della struttura "sockaddr_in" (una per il server e una per la richiesta). Viene inoltre dichiarato un valore di tipo pid_t chiamato "pid".
Queste dichiarazioni sono il preambolo alla definizione del server, che viene eseguita all'interno della funzione "main".
*/
int main(int argc,char **argv) {

  int sockfd,listenfd,n,requestfd;
  struct green_pass greenPass;
  struct sockaddr_in servaddr;
  struct sockaddr_in requestaddr;
  pid_t pid;

/*
Questo codice controlla il numero di argomenti passati al programma. Se il numero di argomenti è diverso da 2, il programma stampa un messaggio di errore su stderr che indica come utilizzare il programma, utilizzando il primo argomento come nome del programma, e poi termina con un codice di uscita 1. Questo implica che il programma richiede un argomento che deve essere l'indirizzo IP del server a cui connettersi.
*/

  if(argc != 2){
    fprintf(stderr,"usage: %s <IPaddress> \n",argv[0]);
    exit(1);
  }


//Questo codice crea un socket di tipo AF_INET per accettare le connessioni in entrata tramite il protocollo TCP (SOCK_STREAM) sulla porta 1026. Il socket viene quindi associato all'indirizzo IP locale e alla porta specificata utilizzando la funzione Bind().
//La linea di codice servaddr.sin_port = htons(1026); imposta la porta di ascolto del socket dell'applicazione a 1026. La funzione htons converte il numero di porta da rappresentazione in ordine di byte dell'host a rappresentazione in ordine di byte di rete. Nella rappresentazione di byte di rete, i byte meno significativi (quelli che indicano il valore della porta) vengono memorizzati per primi, mentre nella rappresentazione di byte dell'host vengono memorizzati per ultimi. La conversione assicura che la rappresentazione dei byte della porta sia corretta sulla rete, indipendentemente dall'architettura della macchina su cui l'applicazione è in esecuzione.

  listenfd = Socket(AF_INET,SOCK_STREAM,0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(1026);
  Bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
  Listen(listenfd,1026);


/*
Questo codice implementa un server che accetta connessioni TCP sulla porta 1026 e gestisce richieste di servizio "green pass" da parte di client.
 Quando una connessione viene accettata, il server crea un nuovo processo figlio per gestire la richiesta del client.
  Il figlio legge il pacchetto di richiesta del client dalla socket e invia la richiesta a un altro server tramite una connessione TCP sulla porta 1025.
   Se la richiesta è per il servizio 0, il figlio legge la risposta del server remoto e la invia al client originale.
Il codice utilizza le funzioni di sistema della libreria di rete BSD per creare socket, accettare connessioni, leggere e scrivere dati sulla rete.
*/
while(1){

sockfd = Accept(listenfd,(struct sockaddr *)NULL,NULL);

if((pid = fork()) < 0 ){
  perror("fork error");
  exit(-1);
}

if(pid == 0){
  close(listenfd);

   while((n = read(sockfd,&greenPass,sizeof(greenPass))) > 0){


  greenPass.num_tesseraSanitaria[21] = 0;

  printf("Tessera Sanitaria: %s\n",greenPass.num_tesseraSanitaria);

  if(n<0){
    fprintf(stderr,"read error\n");
    exit(1);
  }


requestfd = Socket(AF_INET,SOCK_STREAM,0);
requestaddr.sin_family = AF_INET;
requestaddr.sin_port = htons(1025);


  if(inet_pton(AF_INET,argv[1], &requestaddr.sin_addr) < 0) {
    fprintf(stderr,"inet_pton error for %s\n",argv[1]);
    exit(1);
  }
  Connect(requestfd,(struct sockaddr *) &requestaddr,sizeof(requestaddr));
  if( write(requestfd,&greenPass,sizeof(greenPass)) != sizeof(greenPass)) {
    perror("write");
    exit(1);
  }
  printf("Richiesta inviata con successo. \n");
  printf("Servizio: %d\n",greenPass.tipo_Servizio);
  if(greenPass.tipo_Servizio == 0){
int r = read(requestfd,&greenPass,sizeof(greenPass));

if( (r != sizeof(greenPass))  && (r!= sizeof(int))   ){
  perror("read");
  exit(1);
}else if(r < sizeof(greenPass)){
  printf("Il Green Pass non è presente nella banca dati. \n");
}else{

  printf("Tessera Sanitaria: %s\n",greenPass.num_tesseraSanitaria);
  printf("Data di scadenza: %.24s\r\n",ctime(&greenPass.data_Scadenza));
  if( write(sockfd,&greenPass,sizeof(greenPass)) < sizeof(greenPass)){
    perror("write");
    exit(1);
  }
}

close(sockfd);
close(requestfd);
exit(0);

//  Se la richiesta è per il servizio 1 o 2, il figlio esce immediatamente senza fare nulla.
}else if(greenPass.tipo_Servizio == 1 || greenPass.tipo_Servizio ==2){

close(sockfd);
close(requestfd);
exit(0);
}
      }
    }
  }

  /*
  Questo frammento di codice chiude la socket del client che ha terminato la connessione e poi termina il processo figlio. La socket viene chiusa per evitare di tenere connessioni inutilizzate aperte. La chiamata a exit(0) termina il processo figlio in modo ordinato.
  */
  close(sockfd);
  exit(0);
}
