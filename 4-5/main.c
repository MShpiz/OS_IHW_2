#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


char sem_name[] = "/posix-semaphore"; // имя семафора
sem_t *p_sem;                         // адрес семафора

int max_clients;
int workingTime;

int shmid;
const char *shar_object = "posix-shar-object";

int tmp;

void sys_err(char *msg) {
  puts(msg);
  exit(-1);
}

void work() {
  printf("Hairdresser works with a client\n");
  sleep(workingTime);
  printf("Hairdresser finnished working with client\n");
  sem_wait(p_sem);
}
void hairdresser() {
  bool isAsleep = false;
  while (1) {
    int sem_value;
    sem_getvalue(p_sem, &sem_value);
    if (sem_value == 0) {
      if (!isAsleep) {
        tmp = printf("Hairdresser fell asleep\n");
        isAsleep = true;
      }
      // sem_wait(p_sem);
      // sem_post(p_sem);
      continue;
    }
    if (isAsleep) {
      isAsleep = false;
      tmp = printf("Hairdresser woke up\n");
    }
    work();
  }
}

void client() {
  sem_post(p_sem);
  printf("Client %d came in\n", getpid());
  // printf("Client %d is done ", getpid());
}

void stop(int p) {
  if (sem_close(p_sem) == -1) {
    perror("sem_close: Incorrect close of posix semaphore");
    exit(1);
  };
  if (sem_unlink(sem_name) == -1) {
    perror("sem_close: Incorrect unlink of posix semaphore");
    exit(1);
  };
  exit(0);
}

int main(int argc, char *argv[]) {
  (void)signal(SIGINT, stop);
  // printf("here");
  max_clients = atoi(argv[1]);
  workingTime = atoi(argv[2]);
  if ((p_sem = sem_open(sem_name, O_CREAT, 0666, 0)) == 0) {
    perror("sem_open: Can not create posix semaphore");
    exit(1);
  };
  srand(time(NULL)); 

  int child = fork();
  if (child == 0) {

    while (1) {
      int innerClient = fork();
      if (innerClient == 0) {
        int cnt;

        sem_getvalue(p_sem, &cnt);
        if (cnt > max_clients) {
          printf("Saloon is full, client %d left", getpid());
          exit(0);
        }
        client();
        if (sem_close(p_sem) == -1) {
          perror("sem_close: Incorrect close of posix semaphore");
          exit(1);
        };
        exit(0);
      }
      sleep(rand()%20);
    }

  } else {
    hairdresser();
  }
}
