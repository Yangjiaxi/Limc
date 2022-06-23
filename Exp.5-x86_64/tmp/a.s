.intel_syntax noprefix

## Data Section
.data
## String Literals:
.L.str0:
	.asciz	"Q "
.L.str1:
	.asciz	". "
.L.str2:
	.asciz	"\n"

## Global Variables:

## Function Call: print_board
.text
.global	_print_board
_print_board:
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

	mov	[rbp-8],	rdi
	mov	r10,	0
	lea	r11,	[rbp-12]
	mov	[r11],	r10d
.L1:
	lea	r10,	[rbp-12]
	mov	r10d,	[r10]
	mov	r11,	8
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L2
	mov	r10,	0
	lea	r11,	[rbp-16]
	mov	[r11],	r10d
.L4:
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	mov	r11,	8
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L5
	lea	r10,	[rbp-8]
	mov	r10,	[r10]
	lea	r11,	[rbp-12]
	mov	r11d,	[r11]
	mov	rbx,	32
	mov	rax,	rbx
	mul	r11
	mov	r11,	rax
	add	r10,	r11
	lea	r11,	[rbp-16]
	mov	r11d,	[r11]
	mov	rbx,	4
	mov	rax,	rbx
	mul	r11
	mov	r11,	rax
	add	r10,	r11
	mov	r10d,	[r10]
	cmp	r10,	0
	je	.L7
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
	jmp	.L8
.L7:
	lea	r10,	[rip+.L.str1]
	mov	rdi,	r10
	push	r10
	push	r11
	mov	rax,	0
## Call function: printf
	call	_printf
	pop	r11
	pop	r10
	mov	r11,	rax
.L8:
.L6:
	lea	r10,	[rbp-16]
	mov	r11d,	[r10]
	add	r11,	1
	mov	[r10],	r11d
	sub	r11,	1
	jmp	.L4
.L5:
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
.L3:
	lea	r10,	[rbp-12]
	mov	ebx,	[r10]
	add	rbx,	1
	mov	[r10],	ebx
	sub	rbx,	1
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

## Function Call: conflict
.text
.global	_conflict
_conflict:
## Active Record: 
	push	rbp
	mov	rbp,	rsp
## Stack Size: 24
	sub	rsp,	32
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	mov	[rbp-8],	rdi
	mov	[rbp-12],	esi
	mov	[rbp-16],	edx
	mov	r10,	0
	lea	r11,	[rbp-20]
	mov	[r11],	r10d
.L9:
	lea	r10,	[rbp-20]
	mov	r10d,	[r10]
	lea	r11,	[rbp-12]
	mov	r11d,	[r11]
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L10
	lea	r10,	[rbp-8]
	mov	r10,	[r10]
	lea	r11,	[rbp-20]
	mov	r11d,	[r11]
	mov	rbx,	32
	mov	rax,	rbx
	mul	r11
	mov	r11,	rax
	add	r10,	r11
	lea	r11,	[rbp-16]
	mov	r11d,	[r11]
	mov	rbx,	4
	mov	rax,	rbx
	mul	r11
	mov	r11,	rax
	add	r10,	r11
	mov	r10d,	[r10]
	cmp	r10,	0
	je	.L12
	mov	r10,	1
	mov	rax,	r10
	jmp	.Lend1
.L12:
	lea	r10,	[rbp-12]
	mov	r10d,	[r10]
	lea	r11,	[rbp-20]
	mov	r11d,	[r11]
	sub	r10,	r11
	lea	r11,	[rbp-24]
	mov	[r11],	r10d
	mov	r10,	0
	lea	r11,	[rbp-16]
	mov	r11d,	[r11]
	lea	rbx,	[rbp-24]
	mov	ebx,	[rbx]
	sub	r11,	rbx
	mov	rbx,	1
	add	r11,	rbx
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L14
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	lea	rbx,	[rbp-20]
	mov	ebx,	[rbx]
	mov	r12,	32
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	lea	rbx,	[rbp-16]
	mov	ebx,	[rbx]
	lea	r12,	[rbp-24]
	mov	r12d,	[r12]
	sub	rbx,	r12
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	r11d,	[r11]
	mov	r10,	r11
	cmp	r10,	0
	je	.L14
	mov	r10,	1
.L14:
	cmp	r10,	0
	je	.L13
	mov	r10,	1
	mov	rax,	r10
	jmp	.Lend1
.L13:
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	lea	r11,	[rbp-24]
	mov	r11d,	[r11]
	add	r10,	r11
	mov	r11,	8
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L16
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	lea	rbx,	[rbp-20]
	mov	ebx,	[rbx]
	mov	r12,	32
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	lea	rbx,	[rbp-16]
	mov	ebx,	[rbx]
	lea	r12,	[rbp-24]
	mov	r12d,	[r12]
	add	rbx,	r12
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	r11d,	[r11]
	mov	r10,	r11
	cmp	r10,	0
	je	.L16
	mov	r10,	1
.L16:
	cmp	r10,	0
	je	.L15
	mov	r10,	1
	mov	rax,	r10
	jmp	.Lend1
.L15:
.L11:
	lea	r10,	[rbp-20]
	mov	r11d,	[r10]
	add	r11,	1
	mov	[r10],	r11d
	sub	r11,	1
	jmp	.L9
.L10:
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

## Function Call: solve_one
.text
.global	_solve_one
_solve_one:
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

	mov	[rbp-8],	rdi
	mov	[rbp-12],	esi
	lea	r10,	[rbp-12]
	mov	r10d,	[r10]
	mov	r11,	7
	cmp	r10,	r11
	setg	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L17
	lea	r10,	[rbp-8]
	mov	r10,	[r10]
	mov	rdi,	r10
	push	r10
	push	r11
	mov	rax,	0
## Call function: print_board
	call	_print_board
	pop	r11
	pop	r10
	mov	r11,	rax
	mov	r10,	1
	mov	rax,	r10
	jmp	.Lend2
.L17:
	mov	r10,	0
	lea	r11,	[rbp-16]
	mov	[r11],	r10d
.L18:
	lea	r10,	[rbp-16]
	mov	r10d,	[r10]
	mov	r11,	8
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L19
	lea	r10,	[rbp-8]
	mov	r10,	[r10]
	lea	r11,	[rbp-12]
	mov	r11d,	[r11]
	lea	rbx,	[rbp-16]
	mov	ebx,	[rbx]
	mov	rdi,	r10
	mov	rsi,	r11
	mov	rdx,	rbx
	push	r10
	push	r11
	mov	rax,	0
## Call function: conflict
	call	_conflict
	pop	r11
	pop	r10
	mov	r12,	rax
	cmp	r12,	0
	je	.L21
	jmp	.L22
.L21:
	mov	r10,	1
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	lea	rbx,	[rbp-12]
	mov	ebx,	[rbx]
	mov	r12,	32
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	lea	rbx,	[rbp-16]
	mov	ebx,	[rbx]
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
	lea	r10,	[rbp-8]
	mov	r10,	[r10]
	lea	r11,	[rbp-12]
	mov	r11d,	[r11]
	mov	rbx,	1
	add	r11,	rbx
	mov	rdi,	r10
	mov	rsi,	r11
	push	r10
	push	r11
	mov	rax,	0
## Call function: solve_one
	call	_solve_one
	pop	r11
	pop	r10
	mov	rbx,	rax
	cmp	rbx,	0
	je	.L23
	mov	r10,	1
	mov	rax,	r10
	jmp	.Lend2
.L23:
	mov	r10,	0
	lea	r11,	[rbp-8]
	mov	r11,	[r11]
	lea	rbx,	[rbp-12]
	mov	ebx,	[rbx]
	mov	r12,	32
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	lea	rbx,	[rbp-16]
	mov	ebx,	[rbx]
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
.L22:
.L20:
	lea	r10,	[rbp-16]
	mov	r11d,	[r10]
	add	r11,	1
	mov	[r10],	r11d
	sub	r11,	1
	jmp	.L18
.L19:
	mov	r10,	0
	mov	rax,	r10
	jmp	.Lend2

.Lend2:
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
## Stack Size: 264
	sub	rsp,	272
	push	r12
	push	r13
	push	r14
	push	r15
## end of prologue

	mov	r10,	0
	lea	r11,	[rbp-260]
	mov	[r11],	r10d
.L24:
	lea	r10,	[rbp-260]
	mov	r10d,	[r10]
	mov	r11,	8
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L25
	mov	r10,	0
	lea	r11,	[rbp-264]
	mov	[r11],	r10d
.L27:
	lea	r10,	[rbp-264]
	mov	r10d,	[r10]
	mov	r11,	8
	cmp	r10,	r11
	setl	r10b
	movzx	r10,	r10b
	cmp	r10,	0
	je	.L28
	mov	r10,	0
	lea	r11,	[rbp-256]
	lea	rbx,	[rbp-260]
	mov	ebx,	[rbx]
	mov	r12,	32
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	lea	rbx,	[rbp-264]
	mov	ebx,	[rbx]
	mov	r12,	4
	mov	rax,	r12
	mul	rbx
	mov	rbx,	rax
	add	r11,	rbx
	mov	[r11],	r10d
.L29:
	lea	r10,	[rbp-264]
	mov	r11d,	[r10]
	add	r11,	1
	mov	[r10],	r11d
	jmp	.L27
.L28:
.L26:
	lea	r10,	[rbp-260]
	mov	ebx,	[r10]
	add	rbx,	1
	mov	[r10],	ebx
	jmp	.L24
.L25:
	lea	r10,	[rbp-256]
	mov	r12,	0
	mov	rdi,	r10
	mov	rsi,	r12
	push	r10
	push	r11
	mov	rax,	0
## Call function: solve_one
	call	_solve_one
	pop	r11
	pop	r10
	mov	r13,	rax
	mov	r10,	0
	mov	rax,	r10
	jmp	.Lend3

.Lend3:
## end of procedure
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	mov	rsp,	rbp
	pop	rbp
	ret

