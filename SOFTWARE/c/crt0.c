#include <string.h>
extern int main(  int argc, char **argv  ),  _bss_start, _bss_end;
extern void *paws_memset( void *restrict destination, int value, size_t count );
void _start( void ) {
    // CLEAR BSS
    paws_memset( &_bss_start, 0, &_bss_end - &_bss_end );

    // CALL MAIN
    main( 0, NULL );

    // RETURN TO BIOS
}
