OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM   (rw) : ORIGIN = 0x10000000, LENGTH = 64K
}

SECTIONS
{
    . = ORIGIN(FLASH);

    .text : {
        *(.text.start)
        *(.text)
        *(.text.*)
    } > FLASH

    .rodata : ALIGN(4) {
        *(.rodata)
        *(.rodata.*)
    } > FLASH

    .data : ALIGN(4) {
        *(.data)
        *(.data.*)
    } > RAM AT > FLASH

    .bss : ALIGN(4) {
        *(.bss)
        *(.bss.*)
    } > RAM

    _end = .;
}

