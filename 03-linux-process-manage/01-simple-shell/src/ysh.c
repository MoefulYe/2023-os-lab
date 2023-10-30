#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_cmd(char *cmd) {
  if (strcmp(cmd, "exit") == 0) {
    exit(0);
  }
  int pid = fork();
  if (pid == 0) {
    // child
    execlp(cmd, cmd, NULL);
    perror(strerror(errno));
    exit(-1);
  } else {
    // parent
    int status;
    waitpid(pid, &status, 0);
    printf("process %d exited with status %d\n", pid, status);
    return status;
  }
}

void handle_cr(char *line, size_t len) { line[len - 1] = '\0'; }

int repl() {
  char *line;
  size_t len;
  for (;;) {
    printf("ysh> ");
    len = getline(&line, &len, stdin);
    handle_cr(line, len);
    exec_cmd(line);
  }
  return 0;
}

int main(int argc, char *argv[], char *envp[]) { return repl(); }
