#include <assert.h>
#include <fcntl.h>
#include <memory.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUF_SIZE 8 * 1024

// 写互斥
sem_t *write_mutex;
//[读取端, 写入端]
int pipefd[2];
char buf[MAX_BUF_SIZE];

void init() {
  write_mutex = sem_open("read-after-writer-close::write_mutex",
                         O_CREAT | O_RDWR, 0666, 0);
  assert(write_mutex != SEM_FAILED);
  assert(pipe(pipefd) == 0);
  memset(buf, 0, MAX_BUF_SIZE);
}

void drop() {
  close(pipefd[0]);
  close(pipefd[1]);
  sem_close(write_mutex);
  sem_unlink("read-after-writer-close::write_mutex");
}

int child(int id) {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    sem_wait(write_mutex);
    char buf[256];
    sprintf(buf, "a message from child%d\n", id);
    int cnt = write(pipefd[1], buf, strlen(buf));
    printf("child%d: written %d bytes\n", id, cnt);
    sem_post(write_mutex);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  init();
  __auto_type child0 = child(0);
  __auto_type child1 = child(1);
  __auto_type child2 = child(2);
  sem_post(write_mutex);
  close(pipefd[1]);
  waitpid(child0, NULL, 0);
  waitpid(child1, NULL, 0);
  waitpid(child2, NULL, 0);
  int cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
  assert(cnt > 0);
  printf("parent: read %d bytes from pipe:\n%s\n", cnt, buf);
  printf("try to read again(no writer now)\n");
  cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
  assert(cnt == 0);
  bzero(buf, sizeof(buf));
  printf("parent: read %d bytes from pipe: %s\n", cnt, buf);
  printf("cnt == 0 means eof\n");
  drop();
  return 0;
}
