/* linker.ld - Minimal linker script for RV32IMC */

ENTRY(_start)

SECTIONS {
  . = 0x80000000; /* Starting address for code */

  .text : {
    *(.text*)
  }

  .data : {
    *(.data*)
  }

  .bss : {
    *(.bss*)
    *(COMMON)
  }
}

