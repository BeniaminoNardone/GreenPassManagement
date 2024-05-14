/*
Il codice inizia con l'inclusione di alcune librerie standard per la gestione di socket, file e semafori.
In seguito, vengono definite due costanti, una per indicare la scadenza di sei mesi (in secondi) e l'altra per indicare la scadenza di due giorni (in secondi).
*/
#include<sys/types.h> 
#include<unistd.h> 
#include<arpa/inet.h> 
#include<sys/socket.h> 
#include<stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>          
#include <sys/stat.h>
#include "wrapper.h"
#define SCADENZASEIMESI 15552000 //In secondi
#define SCADENZADUEGIORNI 172800 //In secondi

struct green_pass {
  char num_tesseraSanitaria[21];
  time_t data_Scadenza;
  int tipo_servizio;
};

/*
Dentro la funzione principale, vengono dichiarate diverse variabili, tra cui list_fd e conn_fd che rappresentano rispettivamente il file descriptor della socket 
del server e del client, serv_add che rappresenta l'indirizzo del server, received e temp che rappresentano la struttura del green pass ricevuto e quella temporanea, 
pid che rappresenta il processo figlio e zero che viene inizializzato a 0.
*/
int main(int argc, char **argv){
  int list_fd,conn_fd;
  struct sockaddr_in serv_add;
  struct green_pass received;
  struct green_pass temp;
  pid_t pid;
  int zero = 0;
  //viene aperto "file" in modalità lettura/scrittura binaria e viene creato un semaforo con nome "semaphore" con permessi di accesso in lettura e scrittura.
  FILE *database = fopen("database","rb+");
  sem_t* access = sem_open("semaphore",O_CREAT,O_RDWR,1); 


 //Il codice crea un socket TCP IPv4 utilizzando la funzione Socket(). Successivamente, utilizzando la struct serv_add definisce l'indirizzo IP e la porta associata 
 //alla socket tramite la funzione Bind(). Infine, con Listen() viene messa in ascolto la socket per accettare eventuali richieste di connessione in entrata.

  list_fd = Socket(AF_INET,SOCK_STREAM,0);

  serv_add.sin_family = AF_INET;
  serv_add.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_add.sin_port = htons(1025);

  Bind(list_fd,(struct sockaddr *) &serv_add,sizeof(serv_add));

  Listen(list_fd,1025);

/*
Questo codice esegue un ciclo continuo di ascolto per nuove connessioni in ingresso sul socket list_fd. Quando una nuova connessione viene accettata, 
il processo figlio viene creato tramite la funzione fork().
Se la creazione del processo figlio fallisce, viene stampato un messaggio di errore e il programma termina. Se invece la creazione del processo figlio 
ha successo, il figlio viene utilizzato per gestire la nuova connessione e il processo padre torna alla fase di ascolto per nuove connessioni.
Nel figlio, la connessione accettata viene gestita con delle operazioni specifiche, come ad esempio la ricezione e l'invio di dati. 
Quando il figlio ha finito di gestire la connessione, il socket della connessione viene chiuso tramite la funzione close(), e il processo figlio termina.
*/
  while(1){
    conn_fd = Accept(list_fd,(struct sockaddr *) NULL,NULL);

    if((pid = fork()) < 0){
      perror("fork error");
      exit(-1);
    }

    if(pid==0){
      close(list_fd);

/*
Questo codice legge i dati inviati tramite la connessione socket dal client e li memorizza nella variabile received di tipo struct green_pass. 
Se la quantità di dati letti non corrisponde alla dimensione della struct green_pass, viene stampato un messaggio di errore e il programma viene interrotto.
*/
if( read(conn_fd,&received,sizeof(received)) != sizeof(received)){
  perror("read");
  exit(1);
}


/*
Questo codice gestisce la comunicazione tra un server e un client attraverso una socket TCP. Il server accetta connessioni da client, 
legge dati dalla connessione e in base ai dati ricevuti esegue una delle quattro operazioni possibili sui dati di un file contenente informazioni su tessere sanitarie e relative scadenze:
Aggiornamento della scadenza di una tessera sanitaria
Lettura della scadenza di una tessera sanitaria
Convalida di una tessera sanitaria
Invalidamento di una tessera sanitaria
Nella prima parte del codice, il server accetta una connessione dal client e quindi crea un processo figlio per gestire la connessione con il client. 
Successivamente, legge i dati dal socket TCP e li analizza per determinare l'operazione da eseguire sui dati di una tessera sanitaria. 
Questa operazione viene eseguita attraverso una serie di istruzioni switch-case e cicli while che cercano la tessera sanitaria nel file e ne modificano i dati come richiesto. 
Inoltre, in questo codice sono presenti anche istruzioni di sincronizzazione per garantire l'accesso concorrente al file da parte dei processi figli del server. 
Alla fine, il server chiude la connessione con il client e il processo figlio termina.
*/
switch (received.tipo_servizio) {
  case -1:


fseek(database,0,SEEK_SET);
while(fread(&temp,sizeof(struct green_pass),1,database) == 1){
  if(strcmp(temp.num_tesseraSanitaria,received.num_tesseraSanitaria) == 0){
    fseek(database,-(sizeof(struct green_pass)),SEEK_CUR);
    sem_wait(access);
    fwrite(&received,sizeof(struct green_pass),1,database);
    sem_post(access);
    printf("La tessera sanitaria %s è stata aggiornata.\n",received.num_tesseraSanitaria);
    printf("Data di scadenza: %.24s\r\n",ctime(&received.data_Scadenza));

    printf("****************************************************\n");
    close(conn_fd);
    exit(0);
    break;
  }
}

sem_post(access);

  fseek(database,0,SEEK_END);
  sem_wait(access);
  fwrite(&received,sizeof(struct green_pass),1,database);
  sem_post(access);


  printf("Tessera sanitaria: %s\n",received.num_tesseraSanitaria);
  printf("Data di Scadenza: %.24s\r\n",ctime(&received.data_Scadenza));

  printf("****************************************************\n");



      close(conn_fd);
      exit(0);
      break;

  case 0:

   fseek(database,0,SEEK_SET);
  while(fread(&temp,sizeof(struct green_pass),1,database) == 1){

if(strcmp(temp.num_tesseraSanitaria,received.num_tesseraSanitaria) == 0){
  sem_wait(access);
  if( write(conn_fd,&temp,sizeof(struct green_pass)) != sizeof(struct green_pass)){
    sem_post(access);
    perror("write");
    exit(1);
  }else{
    sem_post(access);
    close(conn_fd);
    exit(0);
  }
}
}

    if( write(conn_fd,&zero,sizeof(int)) != sizeof(int))  {
      perror("write");
      exit(1);
    }

  close(conn_fd);
  exit(0);
      break;

      int i = 0;

  case 1:
i = 0;
  printf("La convalida del Green Pass è in corso...\n");

  fseek(database,0,SEEK_SET);
  while(fread(&temp,sizeof(struct green_pass),1,database) == 1){
    if(strcmp(temp.num_tesseraSanitaria,received.num_tesseraSanitaria) == 0){
      if(temp.tipo_servizio == 3){
        temp.data_Scadenza = time(NULL) + SCADENZADUEGIORNI;
      }else{
      temp.data_Scadenza = time(NULL) + SCADENZASEIMESI;
    }
      printf("Il Green Pass è stato convalidato correttamente. \n");
      fseek(database,i*sizeof(struct green_pass),SEEK_SET);
      sem_wait(access);
      fwrite(&temp,sizeof(struct green_pass),1,database);
      sem_post(access);
      close(conn_fd);
      exit(0);
    }else{i++;}
  }
  sem_post(access);

  strcpy(temp.num_tesseraSanitaria,received.num_tesseraSanitaria);
  temp.data_Scadenza = time(NULL) + SCADENZADUEGIORNI;
  temp.tipo_servizio = 3;
  printf("Tessera sanitaria: %s\n",temp.num_tesseraSanitaria);
  printf("Data di scadenza: %.24s\r\n",ctime(&temp.data_Scadenza));
  sem_wait(access);
  fseek(database,0,SEEK_END);
  fwrite(&temp,sizeof(struct green_pass),1,database);
  sem_post(access);

  close(conn_fd);
  exit(0);
    break;

  case 2:

  printf("Annullamento validità del Green Pass in corso...\n");
   i = 0;

   fseek(database,0,SEEK_SET);
   while(fread(&temp,sizeof(struct green_pass),1,database) == 1){
     if(strcmp(temp.num_tesseraSanitaria,received.num_tesseraSanitaria) == 0){
       temp.data_Scadenza = time(NULL) - SCADENZADUEGIORNI;
       printf("Il Green Pass non è più valido.\n");
       fseek(database,i*sizeof(struct green_pass),SEEK_SET);
       sem_wait(access);
       fwrite(&temp,sizeof(struct green_pass),1,database);
       sem_post(access);
       close(conn_fd);
       exit(0);
     }else{i++;}
   }
   sem_post(access);

   strcpy(temp.num_tesseraSanitaria,received.num_tesseraSanitaria);
   temp.data_Scadenza = time(NULL) - SCADENZADUEGIORNI;
   printf("Tessera sanitaria: %s\n",temp.num_tesseraSanitaria);
   printf("Data di scadenza: %.24s\r\n",ctime(&temp.data_Scadenza));
   sem_wait(access);
   fseek(database,0,SEEK_END);
   fwrite(&temp,sizeof(struct green_pass),1,database);
   sem_post(access);
      break;
}
/*
Questo codice è la chiusura del processo figlio che viene eseguita dopo il completamento dell'elaborazione della richiesta del client o quando viene rilevato un errore.
In particolare, chiude il file in cui sono archiviati i dati di green pass, distrugge il semaforo utilizzato per garantire la mutua esclusione nell'accesso al file,
chiude la socket di connessione e termina il processo figlio.
Se la connessione non viene chiusa, il processo figlio entra in un loop infinito che continua ad accettare 
connessioni e a creare nuovi processi figli fino a quando non viene terminato dal processo padre.
*/
fclose(database);
sem_unlink("semaphore");
close(conn_fd);
exit(0);
    }else{close(conn_fd);}
  }
  exit(0);
}
