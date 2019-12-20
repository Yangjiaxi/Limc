.intel_syntax noprefix
.data
.L.str0:
        .ascii "%d\000"
.L.str1:
        .ascii "Num: %d\n\000\000"
.text
.global fn
fn:
        push    rbp
        mov     rbp,    rsp
        sub     rsp,    16
        push    r12
        push    r13
        push    r14
        push    r15
        ## end of prologue

        lea     r10,    [rip+.L.str0]
        lea     r11,    [rbp-4]
        mov     rdi,    r10
        mov     rsi,    r11
        push    r10
        push    r11
        mov     rax,    0
        call    _scanf
        pop     r11
        pop     r10
        mov     rbx,    rax
        lea     r10,    [rip+.L.str1]
        lea     r11,    [rbp-4]
        mov     r11d,   [r11]
        mov     rdi,    r10
        mov     rsi,    r11
        push    r10
        push    r11
        mov     rax,    0
        call    _printf
        pop     r11
        pop     r10
        mov     rbx,    rax
        ## end of procedure

.Lend0:
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        mov     rsp,    rbp
        pop     rbp
        ret
.text
.global _main
_main:
        push    rbp
        mov     rbp,    rsp
        sub     rsp,    0
        push    r12
        push    r13
        push    r14
        push    r15
        ## end of prologue

        push    r10
        push    r11
        mov     rax,    0
        call    fn
        pop     r11
        pop     r10
        mov     r10,    rax
        mov     r10,    0
        mov     rax,    r10
        jmp     .Lend1
        ## end of procedure

.Lend1:
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        mov     rsp,    rbp
        pop     rbp
        ret