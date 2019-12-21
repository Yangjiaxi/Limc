	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15	sdk_version 10, 15
	.intel_syntax noprefix
	.globl	_fn                     ## -- Begin function fn
	.p2align	4, 0x90
_fn:                                    ## @fn
	.cfi_startproc
## %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset rbp, -16
	mov	rbp, rsp
	.cfi_def_cfa_register rbp
	sub	rsp, 16
	lea	rdi, [rip + L_.str]
	lea	rsi, [rip + L_.str.1]
	lea	rdx, [rip + L_.str.2]
	lea	rcx, [rip + L_.str.3]
	lea	r8, [rip + L_.str.4]
	lea	r9, [rip + L_.str.5]
	mov	al, 0
	call	_printf
	mov	dword ptr [rbp - 4], eax ## 4-byte Spill
	add	rsp, 16
	pop	rbp
	ret
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"1"

L_.str.1:                               ## @.str.1
	.asciz	"2"

L_.str.2:                               ## @.str.2
	.asciz	"3"

L_.str.3:                               ## @.str.3
	.asciz	"4"

L_.str.4:                               ## @.str.4
	.asciz	"5"

L_.str.5:                               ## @.str.5
	.asciz	"6"


.subsections_via_symbols
