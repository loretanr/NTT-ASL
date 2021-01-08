	.file	"ntt-radix4-avx0.cpp"
	.section	.text._ZNKSt5ctypeIcE8do_widenEc,"axG",@progbits,_ZNKSt5ctypeIcE8do_widenEc,comdat
	.align 2
	.p2align 4,,15
	.weak	_ZNKSt5ctypeIcE8do_widenEc
	.type	_ZNKSt5ctypeIcE8do_widenEc, @function
_ZNKSt5ctypeIcE8do_widenEc:
.LFB1239:
	.cfi_startproc
	movl	%esi, %eax
	ret
	.cfi_endproc
.LFE1239:
	.size	_ZNKSt5ctypeIcE8do_widenEc, .-_ZNKSt5ctypeIcE8do_widenEc
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"PAPI error: "
	.text
	.p2align 4,,15
	.globl	_Z12handle_errori
	.type	_Z12handle_errori, @function
_Z12handle_errori:
.LFB1485:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	leaq	.LC0(%rip), %rsi
	movl	%edi, %ebx
	leaq	_ZSt4cerr(%rip), %rdi
	movl	$12, %edx
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	leaq	_ZSt4cerr(%rip), %rdi
	movl	%ebx, %esi
	call	_ZNSolsEi@PLT
	movq	%rax, %rbp
	movq	(%rax), %rax
	movq	-24(%rax), %rax
	movq	240(%rbp,%rax), %rbx
	testq	%rbx, %rbx
	je	.L9
	cmpb	$0, 56(%rbx)
	je	.L4
	movsbl	67(%rbx), %esi
.L5:
	movq	%rbp, %rdi
	call	_ZNSo3putEc@PLT
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rax, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	jmp	_ZNSo5flushEv@PLT
	.p2align 4,,10
	.p2align 3
.L4:
	.cfi_restore_state
	movq	%rbx, %rdi
	call	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	(%rbx), %rax
	leaq	_ZNKSt5ctypeIcE8do_widenEc(%rip), %rdx
	movl	$10, %esi
	movq	48(%rax), %rax
	cmpq	%rdx, %rax
	je	.L5
	movq	%rbx, %rdi
	call	*%rax
	movsbl	%al, %esi
	jmp	.L5
.L9:
	call	_ZSt16__throw_bad_castv@PLT
	.cfi_endproc
.LFE1485:
	.size	_Z12handle_errori, .-_Z12handle_errori
	.section	.rodata.str1.1
.LC2:
	.string	"L3 accesses: "
.LC3:
	.string	"L3 misses: "
.LC4:
	.string	"L3 miss/access ratio: "
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB1486:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	$2, %esi
	subq	$48, %rsp
	.cfi_def_cfa_offset 64
	leaq	24(%rsp), %rdi
	movl	$-2147483558, 24(%rsp)
	movl	$-2147483640, 28(%rsp)
	call	PAPI_start_counters@PLT
	testl	%eax, %eax
	jne	.L22
.L11:
	movl	$800000000, %edi
	call	_Znam@PLT
	movdqa	.LC1(%rip), %xmm0
	leaq	800000000(%rax), %rdx
	.p2align 4,,10
	.p2align 3
.L12:
	addq	$16, %rax
	movups	%xmm0, -16(%rax)
	cmpq	%rax, %rdx
	jne	.L12
	leaq	32(%rsp), %rdi
	movl	$2, %esi
	call	PAPI_stop_counters@PLT
	testl	%eax, %eax
	je	.L13
	movl	$1, %edi
	call	_Z12handle_errori
.L13:
	movq	32(%rsp), %rbx
	leaq	.LC2(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	movl	$13, %edx
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	leaq	_ZSt4cout(%rip), %rdi
	movq	%rbx, %rsi
	call	_ZNSo9_M_insertIxEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@PLT
	movq	40(%rsp), %rbx
	leaq	.LC3(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	movl	$11, %edx
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	leaq	_ZSt4cout(%rip), %rdi
	movq	%rbx, %rsi
	call	_ZNSo9_M_insertIxEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@PLT
	pxor	%xmm0, %xmm0
	leaq	.LC4(%rip), %rsi
	pxor	%xmm1, %xmm1
	leaq	_ZSt4cout(%rip), %rdi
	movl	$22, %edx
	cvtsi2sdq	40(%rsp), %xmm0
	cvtsi2sdq	32(%rsp), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, 8(%rsp)
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movsd	8(%rsp), %xmm0
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@PLT
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
.L22:
	.cfi_restore_state
	movl	$1, %edi
	call	_Z12handle_errori
	jmp	.L11
	.cfi_endproc
.LFE1486:
	.size	main, .-main
	.p2align 4,,15
	.type	_GLOBAL__sub_I__Z12handle_errori, @function
_GLOBAL__sub_I__Z12handle_errori:
.LFB1933:
	.cfi_startproc
	leaq	_ZStL8__ioinit(%rip), %rdi
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	call	_ZNSt8ios_base4InitC1Ev@PLT
	movq	_ZNSt8ios_base4InitD1Ev@GOTPCREL(%rip), %rdi
	leaq	__dso_handle(%rip), %rdx
	leaq	_ZStL8__ioinit(%rip), %rsi
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	__cxa_atexit@PLT
	.cfi_endproc
.LFE1933:
	.size	_GLOBAL__sub_I__Z12handle_errori, .-_GLOBAL__sub_I__Z12handle_errori
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z12handle_errori
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC1:
	.long	1
	.long	4
	.long	1
	.long	4
	.hidden	__dso_handle
	.ident	"GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",@progbits
