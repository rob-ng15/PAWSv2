MEMORY
{
   rom (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00001800
   ram (rwx) : ORIGIN = 0x00001800, LENGTH = 0x00000800
}

ENTRY(_start)
STARTUP(build/crt0.o)

SECTIONS {
  .text 0x00000000 : {
        *crt0*(.text)
    } > rom
    .rodata :
    {
        . = ALIGN(8);
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.rodata1)
    } > rom
    .data :
    {
        . = ALIGN(8);
		*(.data .data.* .gnu.linkonce.d.*)
		*(.data1)
        . = ALIGN(8);
		*(.sdata .sdata.* .gnu.linkonce.s.*)
        . = ALIGN(8);
    } > ram
    .bss :
    {
        . = ALIGN(8);
       _bss_start = . ;
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
       . = ALIGN(8);
       _bss_end = . ;
        _heap_start = . ;
    } > ram
}
