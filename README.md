# LIMC

Limc Is a Minimal Compiler.

![HaHa](https://latex.codecogs.com/gif.latex?\bold{\underline{L}imc\quad{}\underline{I}s\quad{}a\quad{}\underline{M}inimal\quad{}\underline{C}ompiler.})

## 郑重鸣谢

- Bison & Flex 的 C++ 项目框架[https://github.com/ezaquarii/bison-flex-cpp-example](https://github.com/ezaquarii/bison-flex-cpp-example)
- 参考了 @winderica [仓库里](https://github.com/winderica/GoodbyeCompiler)的语法分析文件

## 实验环境

- 操作系统：`Mac OS X`
- 开发语言：`C++ 17`
- 集成环境：`CLion & VSCode`

## Part-1 词法分析器

```bash
$ make
$ cat ./test/copy | ./compiler
```

## Part-2 语法分析器

```bash
$ make all
$ make fibo
$ make ss
$ make cmt
$ make copy
```

## Part-3 语义分析

使用`CLion`打开工程

```bash
$ make fibo
$ make ss
$ make cmt
$ make copy
$ make error
```

## Part-4 中间代码生成

使用`CLion`打开工程

```bash
$ make ss
$ make fact
$ make qs
$ make arr
$ make loop
$ make fp
```

## Part-5 目标代码生成

使用`CLion`打开工程

```bash
$ make ss
$ make fact
$ make fib
$ make arr
$ make prime
$ make loop
$ make fp
```
