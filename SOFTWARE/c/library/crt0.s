        .global _start
_start:

	addi sp, sp, -8
	sd ra,  0(sp)

    la t0, _bss_start
    la t1, _bss_end
1:  beq t0,t1,3f
    sd zero,0(t0)
    addi t0,t0,8
    j 1b

3:  li a1,0
	li a0,0
	call main

	ld ra,  0(sp)
	addi sp, sp, 8

    ret

