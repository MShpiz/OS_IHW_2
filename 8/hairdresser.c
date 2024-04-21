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

int workingTime;
int   semid;
char pathname[]=".";
key_t key;
struct sembuf mybuf;

void work() {
  printf("Hairdresser works with a client\n");
  sleep(workingTime);
  printf("Hairdresser finnished working with client\n");
  mybuf.sem_num = 0;
  mybuf.sem_op  = -1;
  mybuf.sem_flg = 0;
  if(semop(semid, &mybuf, 1) < 0){
    printf("Can\'t remove 1 from semaphore\n");
    exit(-1);
  }
}

void hairdresser() {
  bool isAsleep = false;
  while (1) {
    int sem_value = semctl(semid, 0, GETVAL, 0);
    if (sem_value == 0) {
      if (!isAsleep) {
        printf("Hairdresser fell asleep\n");
        isAsleep = true;
      }
      continue;
    }
    if (isAsleep) {
      isAsleep = false;
      printf("Hairdresser woke up\n");
    }
    work();
  }
}

void stop() {
  int err = semctl(semid, 0, IPC_RMID, 0);
  if(err < 0) {
    printf("Incorrect semaphour destroy\n");
  }
  exit(0);
}

int main(int argc, char *argv[], char *envp[])
{
    (void)signal(SIGINT, stop);
    workingTime = atoi(argv[1]);
    key = ftok(pathname, 0);

    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0){
      printf("Can\'t create semaphore set\n");
      exit(-1);
    }

    hairdresser();

    return 0;
}
