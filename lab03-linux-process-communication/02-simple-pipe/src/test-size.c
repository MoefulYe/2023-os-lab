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

void init() { assert(pipe(pipefd) == 0); }

void drop() {
  close(pipefd[0]);
  close(pipefd[1]);
}

// 创建一个子进程, 测试管道的最大容量
int child0() {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    int flags = fcntl(pipefd[1], F_GETFL);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);
    int written = 0;
    for (;;) {
      int cnt = write(pipefd[1], buf, MAX_BUF_SIZE);
      if (cnt == -1) {
        break;
      }
      written += cnt;
    }
    printf("child0: written %d bytes\n", written);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  init();
  waitpid(child0(), NULL, 0);
  int flags = fcntl(pipefd[0], F_GETFL);
  fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
  int read_cnt = 0;
  for (;;) {
    int cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
    if (cnt == -1) {
      break;
    }
    read_cnt += cnt;
  }
  printf("parent: read %d bytes\n", read_cnt);
  fcntl(pipefd[0], F_SETFL, flags);
  close(pipefd[1]);
  close(pipefd[0]);
  drop();
  return 0;
}
