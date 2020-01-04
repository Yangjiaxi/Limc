.intel_syntax noprefix

## Data Section
.data
## String Literals:
.L.str0:
	.asciz	"%5d "
.L.str1:
	.asciz	"\n"
.L.str2:
	.asciz	"\n"

## Global Variables:

## Function Call: PrimeSieve
.text
.global	_PrimeSieve
_PrimeSieve:
## Active Record: 
	push	rbp
	mov	rbp,	rsp
## Stack Size: 20
	sub	rsp,	32
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	mov	[rbp-8],	rdi
	mov	[rbp-12],	esi
	mov	r10,	1
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	mov	rbx,	1
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	mov	rbx,	0
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
	mov	r10,	2
	lea	r11,	[rbp-16]
	mov	[r11],	r10d
.L1:
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	lea	r11,	[rbp-16]
	mov	r11d,	[r11]
	mov	rax,	r11
	mul	r10
	mov	r10,	rax
	lea	r11,	[rbp-12]
	mov	r11d,	[r11]
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L2
	lea	r10,	[rbp-8]
	mov	r10,	[r10]
	lea	r11,	[rbp-16]
	mov	r11d,	[r11]
	mov	rbx,	4
	mov	rax,	rbx
	mul	r11
	mov	r11,	rax
	add	r10,	r11
	mov	r10d,	[r10]
	mov	r11,	0
	cmp	r10,	r11
	sete	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L4
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	mov	r11,	1
	add	r10,	r11
	lea	r11,	[rbp-20]
	mov	[r11],	r10d
.L5:
	lea	r10,	[rbp-20]
	mov	r10d,	[r10]
	lea	r11,	[rbp-12]
	mov	r11d,	[r11]
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L6
	lea	r10,	[rbp-20]
	mov	r10d,	[r10]
	lea	r11,	[rbp-16]
	mov	r11d,	[r11]
	mov	rax,	r10
cqo
	div	r11
	mov	r10,	rdx
	mov	r11,	0
	cmp	r10,	r11
	sete	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L8
	mov	r10,	1
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	lea	rbx,	[rbp-20]
	mov	ebx,	[rbx]
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
.L8:
.L7:
	lea	r10,	[rbp-20]
	mov	r11d,	[r10]
	add	r11,	1
	mov	[r10],	r11d
	jmp	.L5
.L6:
.L4:
.L3:
	lea	r10,	[rbp-16]
	mov	ebx,	[r10]
	add	rbx,	1
	mov	[r10],	ebx
	jmp	.L1
.L2:

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
## Stack Size: 4012
	sub	rsp,	4016
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	mov	r10,	1000
	lea	r11,	[rbp-4]
	mov	[r11],	r10d
	mov	r10,	0
	lea	r11,	[rbp-4008]
	mov	[r11],	r10d
.L9:
	lea	r10,	[rbp-4008]
	mov	r10d,	[r10]
	lea	r11,	[rbp-4]
	mov	r11d,	[r11]
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L10
	mov	r10,	0
	lea	r11,	[rbp-4004]
	lea	rbx,	[rbp-4008]
	mov	ebx,	[rbx]
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
.L11:
	lea	r10,	[rbp-4008]
	mov	r11d,	[r10]
	add	r11,	1
	mov	[r10],	r11d
	sub	r11,	1
	jmp	.L9
.L10:
	lea	r10,	[rbp-4004]
	lea	rbx,	[rbp-4]
	mov	ebx,	[rbx]
	mov	rdi,	r10
	mov	rsi,	rbx
	push	r10
	push	r11
	mov	rax,	0
## Call function: PrimeSieve
	call	_PrimeSieve
	pop	r11
	pop	r10
	mov	r12,	rax
	mov	r10,	0
	lea	rbx,	[rbp-4012]
	mov	[rbx],	r10d
	lea	rbx,	[rbp-4008]
	mov	[rbx],	r10d
.L12:
	lea	r10,	[rbp-4008]
	mov	r10d,	[r10]
	lea	rbx,	[rbp-4]
	mov	ebx,	[rbx]
	cmp	r10,	rbx
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L13
	lea	r10,	[rbp-4004]
	lea	rbx,	[rbp-4008]
	mov	ebx,	[rbx]
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r10,	rbx
	mov	r10d,	[r10]
	mov	rbx,	0
	cmp	r10,	rbx
	sete	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L14
	lea	r10,	[rbp-4012]
	mov	ebx,	[r10]
	add	rbx,	1
	mov	[r10],	ebx
	lea	r10,	[rip+.L.str0]
	lea	rbx,	[rbp-4008]
	mov	ebx,	[rbx]
	mov	rdi,	r10
	mov	rsi,	rbx
	push	r10
	push	r11
	mov	rax,	0
## Call function: printf
	call	_printf
	pop	r11
	pop	r10
	mov	r12,	rax
	lea	r10,	[rbp-4012]
	mov	r10d,	[r10]
	mov	rbx,	5
	mov	rax,	r10
cqo
	div	rbx
	mov	r10,	rdx
	mov	rbx,	0
	cmp	r10,	rbx
	sete	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L15
	lea	r10,	[rip+.L.str1]
	mov	rdi,	r10
	push	r10
	push	r11
	mov	rax,	0
## Call function: printf
	call	_printf
	pop	r11
	pop	r10
	mov	rbx,	rax
.L15:
.L14:
	lea	r10,	[rbp-4008]
	mov	ebx,	[r10]
	add	rbx,	1
	mov	[r10],	ebx
	jmp	.L12
.L13:
	lea	r10,	[rip+.L.str2]
	mov	rdi,	r10
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

