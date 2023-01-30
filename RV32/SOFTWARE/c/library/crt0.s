        .global _start
_start:

	addi sp, sp, -4
	sw ra,  0(sp)

    la t0, _bss_start
    la t1, _bss_end
1:  beq t0,t1,3f
    sw zero,0(t0)
    addi t0,t0,4
    j 1b

3:  li a1,0
	li a0,0
	call main

	lw ra,  0(sp)
	addi sp, sp, 4

    ret

