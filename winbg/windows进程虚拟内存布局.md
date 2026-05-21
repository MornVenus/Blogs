# windows进程虚拟内存布局

+ 今天在调试一个c++程序时突然发现一个局部变量的地址竟然是 `0xB08935F834`，并不符合 `Stack` 位于 `128T` 位置附近的理论。
+ 查询了一番，原来linux平台下基本接近这一理论，但是windows平台下的内存布局更为复杂。

## 虚拟内存布局理论

```
0x000000000000 ~ 0x7FFFFFFFFFFF: 128T 用户空间
	0x00000 ~ 0x0FFFF: 64kb空指针区
	代码区 .text
	数据区 .data
	堆区   heap
	栈区   stack
	
0x800000000000 ~ 0xFFFFFFFFFFFF: 128T 内核空间
```



## 测试

```c++
int main()
{
	int a = 100;
	cout << &a << endl;
	int* p = new int;
	cout << p << endl;
	return 0;
}
```

+ 以上代码如果在linux平台下运行，输出如下

  ```
  0x7ffffb58312c
  0x56229c52b2c0
  ```

  + `stack > heap`，且`stack` 接近128T边界位置，符合理论知识。

+ 如果在windows平台下运行，输入如下

  ```
  000000A4AC36FA54
  0000027DF898C430
  ```

  + `stack < heap`，不符合理论知识。

## 探究栈区

+ `!address -f:Stack`