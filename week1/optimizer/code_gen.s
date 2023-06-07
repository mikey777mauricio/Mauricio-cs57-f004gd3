	.file	"code_gen.s"
	.text
	.globl	fn
	.type	fn, @fntion
fn:
.LFB0:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	pushl	%ebx
	movl	$10, -16(%ebp)
	movl	8(%ebp),%edx
	addl	$10, %edx
	movl	%edx, -8(%ebp)
	movl	%edx, -12(%ebp)
	movl	%edx, -20(%ebp)
	movl	$10, -8(%ebp)
	jmp	.L1
.L1:
	movl	8(%ebp),%edx
	movl	%edx, %ecx
	cmpl	$100, %ecx
	jl	.L2
	jmp	.L3
.L2:
	movl	8(%ebp),%edx
	addl	$5, %edx
	movl	%edx, 8(%ebp)
	movl	%edx, -16(%ebp)
	movl	%edx, -20(%ebp)
	jmp	.L1
.L3:
	movl	-20(%ebp),%edx
	movl	%edx, %ecx
	cmpl	$100, %ecx
	jg	.L4
	jmp	.L5
.L4:
	movl	-16(%ebp),%edx
	movl	%edx, -4(%ebp)
	jmp	.L6
.L5:
	movl	$100, -4(%ebp)
	jmp	.L6
.L6:
	movl	-4(%ebp),%edx
	movl	%edx, %eax
	popl	%ebx
	leave
	ret
