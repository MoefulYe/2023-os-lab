#include <assert.h>
#include <fcntl.h>
#include <memory.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_BUF_SIZE 8 * 1024

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
}

int child(int id) {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    char buf[256];
    printf("child%d: sleeping...\n", id);
    sleep(1);
    sprintf(buf, "a message from child%d\n", id);
    int cnt = write(pipefd[1], buf, strlen(buf));
    printf("child%d: written %d bytes\n", id, cnt);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  init();
  for (int i = 0; i < 3; i++) {
    __auto_type id = child(i);
    printf("main: try to read but empty. blocking now\n");
    int cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
    printf("main: read %d bytes\n", cnt);
  }
  drop();
  return 0;
}
