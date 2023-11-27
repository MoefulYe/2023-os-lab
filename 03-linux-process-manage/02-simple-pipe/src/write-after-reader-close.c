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

// 创建一个子进程, 测试管道的最大容量
int child0() {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    int written = 0;
    write(pipefd[1], "error", strlen("error"));
    printf("child0: written %d bytes\n", written);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  init();
  __auto_type id = child0();
  drop();
  int status;
  waitpid(id, &status, 0);
  assert(status == SIGPIPE);
  printf("parent: child0 exit with status %d, killed because no reader",
         status);
  return 0;
}
