#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "wrapper.h"
#include <time.h>


struct green_pass {
  char num_tesseraSanitaria[21];
  time_t data_Scadenza;
  int tipo_Servizio;
};
// La funzione main prende in input due argomenti dalla riga di comando: l'indirizzo IP del server e il
// numero della tessera sanitaria da verificare. Viene inoltre verificato che il numero di argomenti sia esattamente
// 3 e che il numero della tessera sanitaria abbia una lunghezza valida.
// Successivamente, viene inizializzata la struttura green_pass con il numero della tessera sanitaria passato
// come argomento e il valore 0 per il campo tipo_Servizio. Viene poi creato il socket utilizzando la funzione Socket
// definita nel file wrapper.h, specificando il protocollo AF_INET e il tipo di socket SOCK_STREAM.
// Viene poi inizializzata la struttura servaddr con l'indirizzo IP e la porta del server, specificati come argomenti
// dalla riga di comando.
int main(int argc, char** argv){
  int sockfd;
  struct sockaddr_in servaddr;
  struct green_pass greenPass;

  if(argc != 3){
    fprintf(stderr,"usage: %s <IPaddress> <TesseraSanitaria>\n",argv[0]);
    exit(1);
  }

  if(strlen(argv[2]) != 20){
    fprintf(stderr,"La tessera sanitaria fornita non risulta essere valida. \n");
    exit(1);
  }
  strcpy(greenPass.num_tesseraSanitaria,argv[2]);
  greenPass.tipo_Servizio = 0;
  sockfd = Socket(AF_INET,SOCK_STREAM,0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(1026);

  if(inet_pton(AF_INET,argv[1], &servaddr.sin_addr) < 0) {
    fprintf(stderr,"inet_pton error for %s\n",argv[1]);
    exit(1);
  }
// Viene quindi chiamata la funzione Connect definita nel file wrapper.h per stabilire la connessione con il server,
// passando il socket creato in precedenza e l'indirizzo del server. Viene quindi inviata la struttura green_pass al
// server utilizzando la funzione write. Se la scrittura sul socket non ha successo, viene stampato un messaggio di errore.
Connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
if( write(sockfd,&greenPass,sizeof(greenPass)) != sizeof(greenPass)) {
  perror("write");
  exit(1);
}
// Viene poi stampato un messaggio di conferma che la richiesta è stata inoltrata.
printf("Richiesta inoltrata con successo. \n");
// Successivamente, viene letto il risultato della verifica del green pass dal server utilizzando la funzione read.
// Se la lettura dal socket non ha successo, viene stampato un messaggio di errore.
if( read(sockfd,&greenPass,sizeof(greenPass)) != sizeof(greenPass)){
  perror("read");
  exit(1);
}
// Infine, vengono stampati i risultati della verifica: il numero della tessera sanitaria,
// la data di scadenza e se il green pass è valido o meno in base alla data di scadenza.
printf("Tessera Sanitaria: %s\n",greenPass.num_tesseraSanitaria);
printf("Scadenza: %.24s\r\n",ctime(&greenPass.data_Scadenza));
if(greenPass.data_Scadenza >= time(NULL)){
  printf("Il Green Pass risulta attivo. \n");
}else {
  printf("Il Green Pass non risulta attivo. \n");
}
// Viene quindi chiusa la connessione e il programma termina con un codice di uscita 0.
close(sockfd);
exit(0);
}
