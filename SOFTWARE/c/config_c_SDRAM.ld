MEMORY
{
   ram (rwx) : ORIGIN = 0x4000000, LENGTH = 0x800000
}

ENTRY(_start)

SECTIONS {
  .text 0x4000000 : {
        ../library/build/crt0.o (.text)
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
