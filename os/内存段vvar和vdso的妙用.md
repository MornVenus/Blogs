# 内存段vvar和vdso的妙用

+ 在linux平台下查看一个进程的内存段(`cat /proc/self/maps`)，在末尾你会发现这么两个段: `vvar` 和 `vdso`，在学操作系统的时候，老师只是一笔带过，说这是为减少系统调用设计的段，今天就来看看这两个段是如何减少系统调用的。

```bash
$ cat /proc/self/maps
...
7ffdd1fe7000-7ffdd2008000 rw-p 00000000 00:00 0                          [stack]
7ffdd208d000-7ffdd2091000 r--p 00000000 00:00 0                          [vvar]
7ffdd2091000-7ffdd2093000 r-xp 00000000 00:00 0                          [vdso]
```



## gettimeofday 函数

+ 先来说一个libc的函数: `gettimeofday`，这个函数用于获取时间戳。

  ```c
  #include <stdio.h>
  #include <sys/time.h>
  
  int main()
  {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      printf("seconds: %ld\n", tv.tv_sec);
      return 0;
  }
  
  /**
  * 输出
  */
  seconds: 1779361596
  ```

+ `libc` 的这个函数实现会走两条路径

  + 如果检查 __vdso_gettimeofday 绑定，直接跳转 vdso 实现（用户态执行，无系统调用）
  + 如果没有绑定，则走 syscall 路径

+ 执行以上代码再配合`strace`看是否会调用系统调用: `strace -e gettimeofday ./a.out `

  ```bash
  $ strace -e gettimeofday ./a.out
  seconds: 1779362307
  +++ exited with 0 +++
  ```

## 直接调用gettimeofday系统调用

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/syscall.h>

int sys_gettimeofday(struct timeval* tv, struct timezone* tz)
{
    return syscall(SYS_gettimeofday, tv, tz);
}

int main()
{
    struct timeval tv;
    sys_gettimeofday(&tv, NULL);
    printf("seconds: %ld\n", tv.tv_sec);
    return 0;
}
```

```bash
$ strace -e gettimeofday ./a.out
gettimeofday({tv_sec=1779362323, tv_usec=339161}, NULL) = 0
seconds: 1779362323
+++ exited with 0 +++
```



## 对比以上两种调用耗时

```c
for (int i = 0; i < 1000000; i++)
{
    // gettimeofday
    // syscall gettimeofday
}
```

+ 分别执行100万次耗时时间对比

  ```bash
   ☢  ⚓  ~  time ./libc_time
  
  ________________________________________________________
  Executed in   27.79 millis    fish           external
     usr time   27.35 millis    0.00 micros   27.35 millis
     sys time    0.44 millis  438.00 micros    0.00 millis
  
   ☢  ⚓  ~  time ./syscall_time
  
  ________________________________________________________
  Executed in  556.01 millis    fish           external
     usr time  326.03 millis  323.00 micros  325.71 millis
     sys time  226.55 millis  147.00 micros  226.41 millis
  ```

  + 从以上结果可以看出，系统调用版本要耗时多得多。

## 原理

+ 简单来说，就是将内核中某些系统调用的代码和数据分别映射到用户进程内存的 `vdso` 和 `vvar`段。

+ 比如 `gettimeofday`这个系统调用的伪代码如下

  ```c
  void gettimeofday(struct timeval* tv, struct timezone* tz)
  {
      tv.tv_sec = seconds;
  }
  ```

  + 那么以上这段代码会被映射到 `vdso`段，而所用到的 `seconds` 变量会被映射到 `vvar`段
  + 这样用户进程在调用 `gettimeofday`的时候，只需要调用自己进程内存段 `vdso`中的代码即可。