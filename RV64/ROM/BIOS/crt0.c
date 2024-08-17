extern  int main( int argc, char **argv );
void _start( void ) {
    // SETUP STACKPOINTER
    asm volatile ("li   sp ,0xf70c");
    asm volatile ("lwu  sp,(sp)");
    main(0,0);
}
