#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "wrapper.h"

#define SCADENZASEIMESI 15552000 //in secondi 
// Prima dell'esecuzione del main viene definita una costante chiamata SCADENZASEIMESI che verrà utilizzata in seguito, 
// e una struttura chiamata green_pass che ha tre campi: num_tesseraSanitaria, data_Scadenza, e tipo_servizio.
struct green_pass {
  char num_tesseraSanitaria[21];
  time_t data_Scadenza;
  int tipo_servizio;
};
int main(int argc,char **argv){
// Viene definita una serie di variabili, tra cui tre descrittori di file (listenfd, connfd e serverVfd), 
// due strutture di socket (servCV e serverV) e una variabile di processo (pid).
  int listenfd,connfd,serverVfd;
  int n;
  struct sockaddr_in servCV, serverV;
  struct green_pass greenPass;
  pid_t pid;
// Viene inoltre dichiarato logging e n_client. Se il programma viene eseguito senza un argomento IPaddress, 
// viene stampato un messaggio di errore e il programma termina.
 int logging = 1;
 int n_client = 0;
if(argc != 2){
  fprintf(stderr,"usage: %s <IPaddress>\n",argv[0]);
  exit(1);
}
// Viene creato un socket, listenfd, utilizzando la funzione Socket() con il dominio degli indirizzi AF_INET 
// e il tipo di socket SOCK_STREAM. Viene poi inizializzata la struttura di indirizzo del socket servCV, 
// specificando che la connessione deve essere accettata su qualsiasi indirizzo di rete disponibile (INADDR_ANY) 
// sulla porta 1024. La funzione Bind() viene quindi utilizzata per associare il socket listenfd all'indirizzo servCV. 
// Infine, il socket listenfd viene messo in ascolto con la funzione Listen().
  listenfd = Socket(AF_INET,SOCK_STREAM,0);
  servCV.sin_family = AF_INET;
  servCV.sin_addr.s_addr = htonl(INADDR_ANY);
  servCV.sin_port = htons(1024);
  Bind(listenfd,(struct sockaddr *)&servCV,sizeof(servCV));
  Listen(listenfd,1024);
// Viene avviato un ciclo for infinito in attesa di connessioni in entrata. La funzione Accept viene utilizzata per 
// accettare una connessione in entrata e restituisce un nuovo descrittore del socket connfd. Viene anche mantenuto 
// un contatore di client n_client che si connettono al server.
  while(1) {
      connfd = Accept(listenfd,(struct sockaddr *) NULL,NULL);
    n_client++;
// Viene creato un nuovo processo figlio utilizzando la funzione fork. 
// Se la creazione del processo figlio fallisce, viene generato un errore.
    if((pid = fork()) < 0){
      perror("fork error");
      exit( -1);
    }
// Nel processo figlio, il socket di ascolto listenfd viene chiuso perché non è più necessario. 
// La funzione read viene utilizzata per leggere i dati inviati dal client sul socket connfd nella 
// struttura green_pass.
    if(pid == 0){
        close( listenfd);
      while((n = read(connfd,greenPass.num_tesseraSanitaria,20)) > 0 ) {
        greenPass.num_tesseraSanitaria[n] = 0;
        if (fputs(greenPass.num_tesseraSanitaria,stdout) == EOF) {
          fprintf(stderr,"fputs error\n");
          exit(1);
        }
// Viene assegnato un valore alla variabile data_Scadenza della struttura green_pass aggiungendo 
// alla funzione time il valore SCADENZASEIMESI, che corrisponde a 15552000 secondi, ovvero 6 mesi. 
// La variabile tipo_servizio viene impostata a -1.
        greenPass.data_Scadenza = time(NULL) + SCADENZASEIMESI;
        greenPass.tipo_servizio = -1;
// La variabile n contiene il numero di byte letti. Se n è minore di 0, viene 
// generato un errore. Se la lettura dei dati dal client riesce, vengono scritti sulla console.
        if(n<0){
          fprintf(stderr,"read error\n");
          exit(1);
        }
// Viene creato un nuovo socket serverVfd utilizzando la funzione Socket. 
// Questo socket verrà utilizzato per inviare i dati green_pass al server V.  
// Il server "V" è identificato dall'indirizzo IP passato come argomento dalla riga di comando e dalla porta 1025.
        serverVfd = Socket(AF_INET,SOCK_STREAM,0);
        serverV.sin_family = AF_INET;
        serverV.sin_port = htons(1025);

        if(inet_pton(AF_INET,argv[1], &serverV.sin_addr) < 0) {
          fprintf(stderr, "inet_pton error for %s\n",argv[1]);
          exit(1);
        }
        Connect(serverVfd,(struct sockaddr *)&serverV,sizeof(serverV));
// Dopo la connessione, i dati relativi alla tessera sanitaria vengono scritti sul socket serverVfd utilizzando 
// la funzione write(). Se la scrittura ha successo, viene stampato un messaggio di conferma sulla console, altrimenti 
// viene stampato un messaggio di errore.
        if( write(serverVfd,&greenPass,sizeof(greenPass)) != sizeof(greenPass) ) {
          perror("write");
          exit(1);
        }
        printf(": Il green pass è stato trasmesso con successo al server V.\n");
// Infine, i socket serverVfd e connfd vengono chiusi e il processo figlio termina con una chiamata a exit(). 
// Il processo padre continua ad ascoltare le connessioni in arrivo.
        close(serverVfd);
        close(connfd);
        exit(0);
    }
exit(0);
}else {close(connfd);}
}
}