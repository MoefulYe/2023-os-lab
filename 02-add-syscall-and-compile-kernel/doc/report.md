# 编译内核并添加系统调用

## 下载linux内核

- 下载**4.14.327**到本机

  [Linux内核下载地址](https://kernel.org/)

  ```shell
  ❯ ls
  linux-4.14.327.tar.xz
  ```

- 在当前目录解压

  ```shell
  ❯ tar -xvf linux-4.14.327.tar.xz
  linux-4.14.327/
  linux-4.14.327/.cocciconfig
  linux-4.14.327/.get_maintainer.ignore
  linux-4.14.327/.gitattributes
  linux-4.14.327/.gitignore
  linux-4.14.327/.mailmap
  linux-4.14.327/COPYING
  linux-4.14.327/CREDITS
  linux-4.14.327/Documentation/
  linux-4.14.327/Documentation/.gitignore
  ...
  ```
## 内核源码组织

```shell
❯ tree -L 1
.
├── arch	#平台相关的代码
├── block	#块设备相关的模块
├── certs
├── COPYING
├── CREDITS
├── crypto	#加密散列压缩校验相关的代码
├── Documentation
├── drivers	#设备驱动
├── firmware	#固件
├── fs	#文件系统
├── include	#头文件
├── init	#内核初始化
├── ipc	#进程间通信
├── Kbuil
├── Kconfig
├── kernel
├── lib	#常用数据结构和算法
├── MAINTAINERS
├── Makefile
├── mm	#地址空间
├── net	#网络栈 
├── README
├── samples
├── scripts
├── security
├── sound
├── tools
├── usr
└── virt
```

## **Linux系统调用**相关知识

- 系统调用表

  - 路径`arch/x86/entry/syscalls/syscall_64.tbl`

  - 文件结构`<number> <abi> <name> <entry-point>`

  - 例子

    ```
    #
    # 64-bit system call numbers and entry vectors
    #
    # The format is:
    # <number> <abi> <name> <entry point>
    #
    # The abi is "common", "64" or "x32" for this file.
    #
    0       common  read                    sys_read
    1       common  write                   sys_write
    2       common  open                    sys_open
    3       common  close                   sys_close
    4       common  stat                    sys_newstat
    5       common  fstat                   sys_newfstat
    6       common  lstat                   sys_newlstat
    7       common  poll                    sys_poll
    8       common  lseek                   sys_lseek
    9       common  mmap                    sys_mmap
    10      common  mprotect                sys_mprotect
    11      common  munmap                  sys_munmap
    ...
    ```

- 系统调用服务例程

  - 头文件路径`include/linux/syscalls.h`

  - 服务例程声明格式

    `asmlinkage long sys_statx(int dfd, const char __user *path, unsigned flags,
    			  unsigned mask, struct statx __user *buffer);`

    `asmlinkage`显式地告诉编译器调用参数通过堆栈传递

  - 实现代码路径`kernel/sys.c`

  - 服务例程声明格式

    ```c
    SYSCALL_DEFINE3(setpriority, int, which, int, who, int, niceval) {
        //函数体
    }
    ```

## 编程实现

### 更改系统调用表

```
❯ diff syscall_64.tbl ../linux-4.14.327/arch/x86/entry/syscalls/syscall_64.tbl
342d341
< 333 common  my_set_nice sys_my_set_nice
```

### 编写服务例程

```c
// @params pid 要修改或读取的进程pid
// @params flag 为0读取, 为1时修改
// @params new_nice flag为1时有效, 修改nice值
// @params prio 把旧值写入prio的用户态地址
// @params nice 把旧值写入nice的用户态地址
// @return 0表示成功, -1表示失败
SYSCALL_DEFINE5(sys_my_set_nice, pid_t, pid, int, flag, int, new_nice,
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
```

### 修改头文件

```
asmlinkage long sys_my_set_nice(pid_t pid, int flag, int new_nice,
                                void __user *prio, void __user *nice);
```

### 把修改后的代码整合进源代码中

```shell
❯ make
cp sys.c ../linux-4.14.327/kernel/sys.c
cp syscall_64.tbl ../linux-4.14.327/arch/x86/entry/syscalls/syscall_64.tbl
cp syscalls.h ../linux-4.14.327/include/linux/syscalls.h
```

## 编译内核

- 使用`make menuconfig`文本界面来设置编译选项裁剪内核

  ![](./screenshot.png)

​	最大可能地裁剪内核缩短编译时间

```shell
❯ make -j12
  HOSTCC  scripts/kconfig/conf.o
  HOSTLD  scripts/kconfig/conf
scripts/kconfig/conf  --silentoldconfig Kconfig
#
# configuration written to .config
#
  SYSTBL  arch/x86/include/generated/asm/syscalls_32.h
  SYSHDR  arch/x86/include/generated/asm/unistd_32_ia32.h
  SYSHDR  arch/x86/include/generated/asm/unistd_64_x32.h
  SYSTBL  arch/x86/include/generated/asm/syscalls_64.h
  ...
```

## 出现的问题

### 找不到符号`sys_my_set_nice`

```
ld: arch/x86/entry/syscall_64.o:(.rodata+0xa68): undefined reference to `sys_my_set_nice`
make: *** [Makefile:1049: vmlinux] Error 1
```

系统调用命名不规范

https://zhuanlan.zhihu.com/p/469972204

https://zhuanlan.zhihu.com/p/424240082