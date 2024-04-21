#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

int   semid;
char pathname[]=".";
key_t key;
struct sembuf mybuf;

void client() {
  mybuf.sem_num = 0;
  mybuf.sem_op  = 1;
  mybuf.sem_flg = 0;
  if(semop(semid, &mybuf, 1) < 0){
    printf("Can\'t add 1 to semaphore\n");
    exit(-1);
  }
  printf("Client %d came in\n", getpid());
}

void stop() {
  if(semop(semid, &mybuf, 1) < 0){
    printf("Can\'t add 1 to semaphore\n");
    exit(-1);
  }
  exit(0);
}

int main(int argc, char *argv[])
{
   srand(time(NULL)); 
    (void)signal(SIGINT, stop);
    int max_clients = atoi(argv[1]);
    key = ftok(pathname, 0);

    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
      printf("Can\'t create semaphore set\n");
      exit(-1);
    }
    while (1) {
      sleep(rand()%20);
      if (fork() == 0) {
        int sem_value = semctl(semid, 0, GETVAL, 0);
        if (sem_value > max_clients) {
          printf("client %d left, salon is full\n", getpid());
          exit(0);
        }
        client();
      }
    }

    

    
    return 0;
}
