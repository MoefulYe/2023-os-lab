// @params pid 要修改或读取的进程pid
// @params flag 为0读取, 为1时修改
// @params new_nice flag为1时有效, 修改nice值
// @params prio 把旧值写入prio的用户态地址
// @params nice 把旧值写入nice的用户态地址
// @return 0表示成功, -1表示失败
SYSCALL_DEFINE5(my_set_nice, pid_t, pid, int, flag, int, new_nice,
                void __user *, prio, void __user *, nice) {
  if (flag != 0 && flag != 1) {
    return -1;
  }
  if (new_nice > 19 || new_nice < -20) {
    return -1;
  }
  struct task_struct *task;
  struct pid *pid_struct;
  int old_nice;
  int old_prio;
  pid_struct = find_get_pid(pid);
  if (pid_struct == NULL) {
    return -1;
  }
  task = pid_task(pid_struct, PIDTYPE_PID);
  if (task == NULL) {
    return -1;
  }
  old_nice = task_nice(task);
  old_prio = task_prio(task);
  if (flag == 1) {
    set_user_nice(task, new_nice);
  }
  copy_to_user(prio, &old_prio, sizeof(int));
  copy_to_user(nice, &old_nice, sizeof(int));
  return 0;
}
