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

// 写互斥
sem_t *write_mutex;
// 子进程1和父进程同步
sem_t *sync1;
// 子进程2和父进程同步
sem_t *sync2;
//[读取端, 写入端]
int pipefd[2];
char buf[MAX_BUF_SIZE];

void init() {
  write_mutex =
      sem_open("simple-pipe-test::write_mutex", O_CREAT | O_RDWR, 0666, 0);
  assert(write_mutex != SEM_FAILED);
  sync1 = sem_open("simple-pipe-test::sync1", O_CREAT | O_RDWR, 0666, 0);
  assert(sync1 != SEM_FAILED);
  sync2 = sem_open("simple-pipe-test::sync2", O_CREAT | O_RDWR, 0666, 0);
  assert(sync2 != SEM_FAILED);
  assert(pipe(pipefd) == 0);
  memset(buf, 0, MAX_BUF_SIZE);
}

void drop() {
  close(pipefd[0]);
  close(pipefd[1]);
  sem_close(write_mutex);
  sem_close(sync1);
  sem_close(sync2);
  sem_unlink("simple-pipe-test::write_mutex");
  sem_unlink("simple-pipe-test::sync1");
  sem_unlink("simple-pipe-test::sync2");
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

int child1() {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    sem_wait(write_mutex);
    int cnt = write(pipefd[1], "a message from child1\n", 22);
    printf("child1: written %d bytes\n", cnt);
    sem_post(write_mutex);
    sem_post(sync1);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int child2() {
  int pid = fork();
  if (pid == 0) {
    close(pipefd[0]);
    sem_wait(write_mutex);
    int cnt = write(pipefd[1], "a message from child2\n", 22);
    printf("child2: written %d bytes\n", cnt);
    sem_post(write_mutex);
    sem_post(sync2);
    close(pipefd[1]);
    exit(0);
  }
  return pid;
}

int main(int argc, char *argv[]) {
  init();
  assert(child1() != 0);
  assert(child2() != 0);
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
  sem_post(write_mutex);
  sem_wait(sync1);
  sem_wait(sync2);
  int cnt = read(pipefd[0], buf, MAX_BUF_SIZE);
  printf("parent: read %d bytes\n", cnt);
  for (int i = 0; i < cnt; i++) {
    putchar(buf[i]);
  }
  drop();
  return 0;
}
