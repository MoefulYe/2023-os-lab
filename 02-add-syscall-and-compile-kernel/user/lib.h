#include <signal.h>

extern int get_nice(pid_t pid, int *prio, int *nice);
extern int set_nice(pid_t pid, int new_nice, int *prio, int *nice);
