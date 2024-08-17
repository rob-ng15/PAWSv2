extern  int main( int argc, char **argv );
void _start( void ) {
    // SETUP STACKPOINTER
    asm volatile ("li sp ,0x1000");
    main(0,0);
}
