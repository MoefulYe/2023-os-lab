#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ashenye");
MODULE_DESCRIPTION("print all kernel threads info");
MODULE_VERSION("0.1");

static pid_t pid;
module_param(pid, int, 0644);

static int __init print_info_init(void) {
  struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
  struct list_head *i;
  struct task_struct *t;
  if (p == NULL) {
    printk(KERN_INFO "print-family-info: "
                     "pid %d not found",
           pid);
    return 0;
  }
  printk(KERN_INFO "print-family-info: my name: %s, pid: %d, state: %ld",
         p->comm, p->pid, p->stats);
  if (p->parent == NULL) {
    printk(KERN_INFO "print-family-info: I dont have a parent");
  } else {
    printk(KERN_INFO "print-family-info: my parent: %s, pid: %d, state: %ld",
           p->parent->comm, p->parent->pid, p->parent->stats);
    list_for_each(i, &p->parent->children) {
      t = list_entry(i, struct task_struct, sibling);
      printk(KERN_INFO "print-family-info: my sibling: %s, pid: %d, state: %ld",
             t->comm, t->pid, t->stats);
    }
  }
  list_for_each(i, &p->children) {
    t = list_entry(i, struct task_struct, sibling);
    printk(KERN_INFO "print-family-info: my child: %s, pid: %d, state: %ld",
           t->comm, t->pid, t->stats);
  }
  return 0;
}

static void __exit print_info_exit(void) {}

module_init(print_info_init);
module_exit(print_info_exit);
