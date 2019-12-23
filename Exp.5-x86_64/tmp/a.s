.intel_syntax noprefix

## Data Section
.data
## String Literals:
.L.str0:
	.asciz	"Input n: "
.L.str1:
	.asciz	"%d"
.L.str2:
	.asciz	"%d! = %d\n"

## Global Variables:

## Function Call: fact
.text
.global	_fact
_fact:
## Active Record: 
	push	rbp
	mov	rbp,	rsp
## Stack Size: 4
	sub	rsp,	16
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	mov	[rbp-4],	edi
	lea	r10,	[rbp-4]
	mov	r10d,	[r10]
	mov	r11,	1
	cmp	r10,	r11
	setle	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L1
	mov	r10,	1
	mov	rax,	r10
	jmp	.Lend0
.L1:
	lea	r10,	[rbp-4]
	mov	r10d,	[r10]
	lea	r11,	[rbp-4]
	mov	r11d,	[r11]
	mov	rbx,	1
	sub	r11,	rbx
	mov	rdi,	r11
	push	r10
	push	r11
	mov	rax,	0
## Call function: fact
	call	_fact
	pop	r11
	pop	r10
	mov	rbx,	rax
	mov	rax,	rbx
	mul	r10
	mov	r10,	rax
	mov	rax,	r10
	jmp	.Lend0

.Lend0:
## end of procedure
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	mov	rsp,	rbp
	pop	rbp
	ret

## Function Call: main
.text
.global	_main
_main:
## Active Record: 
	push	rbp
	mov	rbp,	rsp
## Stack Size: 4
	sub	rsp,	16
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	lea	r10,	[rip+.L.str0]
	mov	rdi,	r10
	push	r10
	push	r11
	mov	rax,	0
## Call function: printf
	call	_printf
	pop	r11
	pop	r10
	mov	r11,	rax
	lea	r10,	[rip+.L.str1]
	lea	r11,	[rbp-4]
	mov	rdi,	r10
	mov	rsi,	r11
	push	r10
	push	r11
	mov	rax,	0
## Call function: scanf
	call	_scanf
	pop	r11
	pop	r10
	mov	rbx,	rax
	lea	r10,	[rip+.L.str2]
	lea	r11,	[rbp-4]
	mov	r11d,	[r11]
	lea	rbx,	[rbp-4]
	mov	ebx,	[rbx]
	mov	rdi,	rbx
	push	r10
	push	r11
	mov	rax,	0
## Call function: fact
	call	_fact
	pop	r11
	pop	r10
	mov	r12,	rax
	mov	rdi,	r10
	mov	rsi,	r11
	mov	rdx,	r12
	push	r10
	push	r11
	mov	rax,	0
## Call function: printf
	call	_printf
	pop	r11
	pop	r10
	mov	rbx,	rax
	mov	r10,	0
	mov	rax,	r10
	jmp	.Lend1

.Lend1:
## end of procedure
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	mov	rsp,	rbp
	pop	rbp
	ret

