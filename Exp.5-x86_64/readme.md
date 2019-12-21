# Limc

Limc Is a Minimal Compiler

## 目标代码生成 `x86-64`

### `x86-64`寄存器描述

`x86-64`拥有16个64位寄存器：

- `RAX`
- `RBX`
- `RCX`
- `RDX`
- `RDI`
- `RSI`
- `RBP`
- `RSP`
- `R8-15`

如何使用：

首先将寄存器分为四类：

- 用于保存中间结果
- 用于传递参数
- 用于各种通用计算
- 栈帧、跳转相关

由于想要利用C语言的ABI，因此参数传递必须遵守C语言的标准，实验如下：

```c++
void fn(int a, int b, int c, int d, int e, int f, int g) { return; }

int main() {
    fn(0, 0, 0, 0, 0, 0, 0);
    return 0;
}
```

生成GAS代码：

`$ gcc -S -masm=intel a.c`

```asm

...

	mov	dword ptr [rbp - 4], edi
	mov	dword ptr [rbp - 8], esi
	mov	dword ptr [rbp - 12], edx
	mov	dword ptr [rbp - 16], ecx
	mov	dword ptr [rbp - 20], r8d
	mov	dword ptr [rbp - 24], r9d
	mov	dword ptr [rbp - 28], eax ## 4-byte Spill

...

_main:   

...

	mov	edi, eax
	mov	esi, eax
	mov	edx, eax
	mov	ecx, eax
	mov	r8d, eax
	mov	r9d, eax
	mov	dword ptr [rsp], 0
	call	_fn

...

```

可见对于前六个参数依次使用了`edi`, `esi`, `edx`, `ecx`, `r8d`, `r9d`

再进行实验

```c++
void fn() { printf("a", 2, 3, 4, 5, 6); }
```

有

```asm
...

	.cfi_def_cfa_register rbp
	sub	rsp, 16
	lea	rdi, [rip + L_.str]
	mov	esi, 2
	mov	edx, 3
	mov	ecx, 4
	mov	r8d, 5
	mov	r9d, 6
	mov	al, 0
	call	_printf

...

```

则使用了`rdi`, `esi`, `edx`, `ecx`, `r8d`, `r9d`

若`void fn() { printf("1", "2", "3", "4", "5", "6"); }`则有

```
	sub	rsp, 16
	lea	rdi, [rip + L_.str]
	lea	rsi, [rip + L_.str.1]
	lea	rdx, [rip + L_.str.2]
	lea	rcx, [rip + L_.str.3]
	lea	r8, [rip + L_.str.4]
	lea	r9, [rip + L_.str.5]
	mov	al, 0
```

比较可见，这三次都是在使用`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`的整体或一部分，因此此次试验我也打算这样使用参数寄存器

也可以通过`syscall`证明[https://wiki.cdot.senecacollege.ca/wiki/Syscalls#x86_64-bit](https://wiki.cdot.senecacollege.ca/wiki/Syscalls#x86_64-bit)

此外，随意使用的寄存器或函数返回值存储根据国际惯例设定为`RAX`，栈顶寄存器和基址寄存器也按照规定使用`RSP`与`RBP`，其余都用作中间变量寄存器。

因此，总结一下：

|  REG   |      USE      |
| :----: | :-----------: |
|  rax   |    General    |
|  rbp   | Base Pointer  |
|  rsp   | Stack Pointer |
|  rdi   |    arg[1]     |
|  rsi   |    arg[2]     |
|  rdx   |    arg[3]     |
|  rcx   |    arg[4]     |
|   r8   |    arg[5]     |
|   r9   |    arg[6]     |
|  rbx   |  Tmp Res[1]   |
| r10-15 | Tmp Res[2-7]  |

六个参数寄存器，七个中间结果寄存器

根据中间代码生成思想：

> 中间运算结果的生命周期很短，且单独一个表达式不会涉及很多寄存器

我认为七个寄存器已经够用了
