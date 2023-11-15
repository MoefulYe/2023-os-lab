#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

_Bool is_exit(char *str) { return strcmp("exit\n", str) == 0; }

int main(int argc, char *argv[]) {
  sem_t *full = sem_open("shm_full", O_CREAT | O_RDWR, 0666, 0);
  sem_t *empty = sem_open("shm_empty", O_CREAT | O_RDWR, 0666, 1);
  sem_t *over = sem_open("shm_over", O_CREAT | O_RDWR, 0666, 0);

  key_t sm_id = ftok("04-shared-memory", 1);
  int shm_id = shmget(sm_id, 1024, IPC_CREAT | 0666);
  char *shm_ptr = (char *)shmat(shm_id, NULL, 0);

  for (;;) {
    sem_wait(empty);
    printf("sender> ");
    size_t len = 0;
    char *line;
    len = getline(&line, &len, stdin);
    memset(shm_ptr, 0, 1024);
    memcpy(shm_ptr, line, len);
    sem_post(full);
    if (is_exit(line)) {
      break;
    }
  }
  sem_wait(over);
  printf("sender> %s", shm_ptr);
  shmdt(shm_ptr);
  shmctl(shm_id, IPC_RMID, NULL);
  sem_close(full);
  sem_close(empty);
  sem_close(over);
  sem_unlink("shm_full");
  sem_unlink("shm_empty");
  sem_unlink("shm_over");
  return 0;
}
