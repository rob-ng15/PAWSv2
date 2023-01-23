extern int main(  int argc, char **argv  ),  _bss_start, _bss_end;
void _start( void ) {
    // CLEAR BSS
    int volatile *DMA_REGS = (int volatile *) 0xfe00;
    unsigned char volatile *DMA_REGS_B = (unsigned char volatile *) 0xfe00;
    DMA_REGS_B[0x0e] = (unsigned char)0; DMA_REGS[0] = (long)0xfe0e; DMA_REGS[1] = (long)&_bss_start; DMA_REGS[2] = (&_bss_end - &_bss_start); DMA_REGS_B[0x0c] = 4;

    // CALL MAIN
    main( 0, 0 );

    // RETURN TO BIOS
}
