#! /bin/sh

set -e

echo "Adding system call..."
 
#修改头文件
MY_SET_NICE_DECLARATION="asmlinkage long sys_my_set_nice(pid_t pid, int flag, int new_nice, void __user *prio, void __user *nice);"
grep --silent -F "$MY_SET_NICE_DECLARATION" work/kernel/linux-5.15.137/include/linux/syscalls.h || echo "$MY_SET_NICE_DECLARATION" >> work/kernel/linux-5.15.137/include/linux/syscalls.h

#修改系统调用表
ADD_SYSCALL_ENTRY="335 64 my_set_nice sys_my_set_nice"
grep --silent -F "$ADD_SYSCALL_ENTRY" work/kernel/linux-5.15.137/arch/x86/entry/syscalls/syscall_64.tbl || cat my_set_nice/syscall_64.tbl > work/kernel/linux-5.15.137/arch/x86/entry/syscalls/syscall_64.tbl

#修改系统调用实现
MY_SET_NICE_DEFINE=$(cat my_set_nice/my_set_nice.c)
grep --silent -F "my_set_nice" work/kernel/linux-5.15.137/kernel/sys.c || echo "$MY_SET_NICE_DEFINE" >> work/kernel/linux-5.15.137/kernel/sys.c

echo "done..."
