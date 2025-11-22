.section .rodata
msg:    .string "hello, world\n"
	len = . - msg

	.section .text
	.globl _start
_start:
	    li a0, 1
	    la a1, msg
	    li a2, len
	    li a7, 64
	    ecall

	    li a0, 0
	    li a7, 93
	    ecall
