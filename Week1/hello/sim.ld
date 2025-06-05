SECTIONS
{
  . = 0x1000; /* start at a valid address */
  .text : { *(.text*) }
  .data : { *(.data*) }
  .bss  : { *(.bss*)  }
  .rodata : { *(.rodata*) }
}

