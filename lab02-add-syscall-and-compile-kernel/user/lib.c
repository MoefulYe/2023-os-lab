#include <signal.h>
#include <unistd.h>
#define MY_SET_NICE 335

int get_nice(pid_t pid, int *prio, int *nice) {
  return syscall(MY_SET_NICE, pid, 0, 0, prio, nice);
}

int set_nice(pid_t pid, int new_nice, int *prio, int *nice) {
  return syscall(MY_SET_NICE, pid, 1, new_nice, prio, nice);
}
