MEMORY
{
   ram (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00004000
}

ENTRY(_start)
STARTUP(build/crt0.o)

SECTIONS {
  .text 0x00000000 : {
        *(.text*)
    } > ram
    .bss :
    {
       _bss_start = . ;
       *(.bss*)
       *(COMMON)
       _bss_end = . ;
    } > ram
    .sdata :
    {
        *(.sdata*)
    } > ram
    .data :
    {
        *(.data*)
    } > ram
    .srodata :
    {
        *(.srodata*)
    } > ram
    .rodata :
    {
        *(.rodata*)
        _heap_start = . ;
    } > ram
}
