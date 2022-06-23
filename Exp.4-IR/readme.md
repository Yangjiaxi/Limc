# Limc

Limc Is a Minimal Compiler

## 中间代码生成

|           IR指令 |      含义      |  **`lhs`**  |     rhs     |   送入    |       附加数据        | 类型              |
| ---------------: | :------------: | :---------: | :---------: | :-------: | :-------------------: | :---------------- |
|        **`Add`** |     算术加     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Sub`** |     算术减     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Mul`** |     算术乘     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Div`** |     算术除     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|     **`AddImm`** |    加寄存器    |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegImm`     |
|     **`SubImm`** |     寄存器     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegImm`     |
|        **`Imm`** |     立即数     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-Imm`        |
|   **`BpOffset`** |  栈寻址获得EA  |    `reg`    |    `imm`    | **`lhs`** |           /           | `Reg<-Imm`        |
|        **`Mov`** |      送入      |    `reg`    | `reg`/`imm` | **`lhs`** |           /           | `Reg<-Reg/Imm`    |
|     **`Return`** |      返回      | `reg`<结果> |      /      |   `rax`   |           /           | `Reg<-rax`        |
|       **`Call`** |      调用      |    `reg`    |      /      | **`lhs`** | 名、栈大小、`argregs` | `Reg<-(call+Reg)` |
|      **`Label`** |    放置标号    |   `label`   |      /      |     /     |           /           | `Label`           |
|  **`LabelAddr`** |   获得标号EA   |    `reg`    |      /      | **`lhs`** |       `label`名       | `Reg<-Addr`       |
|         **`EQ`** |    关系等于    |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|         **`NE`** |   关系不等于   |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|         **`LE`** |  关系小于等于  |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|         **`LT`** |    关系小于    |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|         **`GE`** |  关系大于等于  |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|         **`GT`** |    关系大于    |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`And`** |     逻辑与     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|         **`Or`** |     逻辑或     |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Xor`** |    按位异或    |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Shl`** |      左移      |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Shr`** |      右移      |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Mod`** |      取模      |    `reg`    |    `reg`    | **`lhs`** |           /           | `Reg<-RegReg`     |
|        **`Neg`** |      取负      |    `reg`    |      /      | **`lhs`** |           /           | `Reg<-Reg`        |
|        **`Jmp`** |   无条件跳转   |   `label`   |      /      |     /     |           /           | `Jump`            |
|         **`If`** |   若测试为真   |    `reg`    |   `label`   |     /     |           /           | `Reg+Jump`        |
|     **`Unless`** |   若测试为假   |    `reg`    |   `label`   |     /     |           /           | `Reg+Jump`        |
|       **`Load`** |   加载内存值   |    `reg`    |    `mem`    | **`lhs`** |       数据长度        | `Reg<-Mem`        |
|      **`Store`** |   保存入内存   |    `mem`    |    `reg`    |     /     |       数据长度        | `Mem<-Reg`        |
| **`StoreParam`** |  保存调用参数  |    `mem`    |    `reg`    |     /     |       数据长度        | `Mem<-Reg`        |
|       **`Kill`** | 寄存器使用完毕 |    `reg`    |      /      |     /     |           /           | `Reg`             |
|        **`Nop`** |     无操作     |      /      |      /      |     /     |           /           | /                 |
|    **`Commenm`** |      注释      |      /      |      /      |     /     |           /           | /                 |
