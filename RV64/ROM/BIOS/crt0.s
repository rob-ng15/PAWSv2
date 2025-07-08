        .global _start
_start:
    csrwi   mstatus,0
    csrwi   mie,0
    csrwi   mip,0

    li  sp,0xf70c
    lwu sp,(sp)

    la t0, _bss_start
    la t1, _bss_end
1:  beq t0,t1,3f
    sd zero,0(t0)
    addi t0,t0,8
    j 1b

3:  li  a1,0
	li  a0,0
	call main

	j  _start
