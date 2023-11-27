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

#define MAX_BUF_SIZE 16 * 4096

//[读取端, 写入端]
int pipefd[2];
char buf[MAX_BUF_SIZE];

void init() {
  assert(pipe(pipefd) == 0);
  memset(buf, 0, MAX_BUF_SIZE);
}

void drop() {
  close(pipefd[0]);
  close(pipefd[1]);
  sem_unlink("write-when-pipe-full::write_mutex");
}

int child(int id) {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    char buf[8 * 4096];
    sprintf(buf, "a message from child%d\n", id);
    int cnt = write(pipefd[1], buf, sizeof(buf));
    printf("child%d: written %d bytes\n", id, cnt);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  init();
  waitpid(child(0), NULL, 0);
  waitpid(child(1), NULL, 0);
  __auto_type child2 = child(2);
  printf("child2: try to write but pipe is full\n");
  close(pipefd[1]);
  printf("main: sleeping ...\n");
  sleep(1);
  printf("main: wakeup ...\n");
  printf("main: clear pipe, child2 can be unblocked now\n");
  int cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
  assert(cnt == MAX_BUF_SIZE);
  waitpid(child2, NULL, 0);
  cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
  assert(cnt == 8 * 4096);
  printf("main: recv from child2");
  drop();
  return 0;
}
