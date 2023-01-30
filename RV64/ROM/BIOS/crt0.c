extern  int main( int argc, char **argv );
void _start( void ) {
    // SETUP STACKPOINTER
    asm volatile ("li sp ,0x7f00");
    main(0,0);
}
