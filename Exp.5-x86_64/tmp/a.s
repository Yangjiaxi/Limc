.intel_syntax noprefix

## Data Section
.data
## String Literals:
.L.str0:
	.asciz	"[a^b] Input a b: "
.L.str1:
	.asciz	"%d%d"
.L.str2:
	.asciz	"%d ^ %d = %d\n"

## Global Variables:

## Function Call: fast_pow
.text
.global	_fast_pow
_fast_pow:
## Active Record: 
	push	rbp
	mov	rbp,	rsp
## Stack Size: 16
	sub	rsp,	16
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	mov	[rbp-4],	edi
	mov	[rbp-8],	esi
	mov	r10,	1
	lea	r11,	[rbp-12]
	mov	[r11],	r10d
	lea	r10,	[rbp-4]
	mov	r10d,	[r10]
	lea	r11,	[rbp-16]
	mov	[r11],	r10d
.L1:
	lea	r10,	[rbp-8]
	mov	r10d,	[r10]
	cmp	r10,	0
	je	.L2
	lea	r10,	[rbp-8]
	mov	r10d,	[r10]
	mov	r11,	1
	and	r10,	r11
	cmp	r10,	0
	je	.L3
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	lea	r11,	[rbp-12]
	mov	ebx,	[r11]
	mov	rax,	r10
	mul	rbx
	mov	rbx,	rax
	mov	[r11],	ebx
.L3:
	mov	r10,	1
	lea	r11,	[rbp-8]
	mov	ebx,	[r11]
	mov	cl,	r10b
	shr	rbx,	cl
	mov	[r11],	ebx
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	lea	r11,	[rbp-16]
	mov	ebx,	[r11]
	mov	rax,	r10
	mul	rbx
	mov	rbx,	rax
	mov	[r11],	ebx
	jmp	.L1
.L2:
	lea	r10,	[rbp-12]
	mov	r10d,	[r10]
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
## Stack Size: 12
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
	lea	rbx,	[rbp-8]
	mov	rdi,	r10
	mov	rsi,	r11
	mov	rdx,	rbx
	push	r10
	push	r11
	mov	rax,	0
## Call function: scanf
	call	_scanf
	pop	r11
	pop	r10
	mov	r12,	rax
	lea	r10,	[rbp-4]
	mov	r10d,	[r10]
	lea	r11,	[rbp-8]
	mov	r11d,	[r11]
	mov	rdi,	r10
	mov	rsi,	r11
	push	r10
	push	r11
	mov	rax,	0
## Call function: fast_pow
	call	_fast_pow
	pop	r11
	pop	r10
	mov	rbx,	rax
	lea	r10,	[rbp-12]
	mov	[r10],	ebx
	lea	r10,	[rip+.L.str2]
	lea	r11,	[rbp-4]
	mov	r11d,	[r11]
	lea	rbx,	[rbp-8]
	mov	ebx,	[rbx]
	lea	r12,	[rbp-12]
	mov	r12d,	[r12]
	mov	rdi,	r10
	mov	rsi,	r11
	mov	rdx,	rbx
	mov	rcx,	r12
	push	r10
	push	r11
	mov	rax,	0
## Call function: printf
	call	_printf
	pop	r11
	pop	r10
	mov	r13,	rax
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

