extern  int main( void ),  _bss_start, _bss_end;
void _start( void ) {
    // CLEAR BSS
    memset( &_bss_start, 0, &_bss_end - &_bss_end );

    // CALL MAIN
    main();

    // RETURN TO BIOS
}
