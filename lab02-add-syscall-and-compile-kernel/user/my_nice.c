#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// my_nice set <pid> <new-nice>
// my_nice get <pid>
int main(int argc, char *argv[]) {
  if (argc > 4 || argc < 3) {
    printf("The number of invalid args\n");
    return -1;
  }
  char *sub = argv[1];
  if (strcmp(sub, "get") == 0) {
    char *pid_s = argv[2];
    int pid = atoi(pid_s);
    int nice;
    int prio;
    if (get_nice(pid, &prio, &nice) == -1) {
      printf("err\n");
      return -1;
    }
    printf("pid: %d, prio: %d, nice: %d\n", pid, prio, nice);
  } else if (strcmp(sub, "set") == 0) {
    char *pid_s = argv[2];
    char *new_nice_s = argv[3];
    int pid = atoi(pid_s);
    int new_nice = atoi(new_nice_s);
    int prio;
    int nice;
    if (set_nice(pid, new_nice, &prio, &nice) == -1) {
      printf("err\n");
      return -1;
    }
    printf("pid: %d, old_prio: %d, old_nice: %d, new_nice: %d\n", pid, prio,
           nice, new_nice);
  } else {
    printf("unsupported subcommand\b");
    return -1;
  }
  return 0;
}
