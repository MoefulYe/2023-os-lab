#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ashenye");
MODULE_DESCRIPTION("print all kernel threads info");
MODULE_VERSION("0.1");

static int __init print_info_init(void) {
  // struct task_struct *p = &init_task;
  struct task_struct *p;
  for_each_process(p) if (p->mm == NULL) {
    printk(KERN_INFO "print-kernel-threads-info: "
                     "name: %s, pid: %d, state: %ld, prio: %d, parent: %d",
           p->comm, p->pid, p->stats, p->prio, p->parent->pid);
  }
  return 0;
}

static void __exit print_info_exit(void) {}

module_init(print_info_init);
module_exit(print_info_exit);
