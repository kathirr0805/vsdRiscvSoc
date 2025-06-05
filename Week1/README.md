RISC-V Bare-Metal Programming Guide
This repository provides a comprehensive guide to bare-metal programming on RISC-V (RV32IMAC) using the RISC-V GNU Toolchain, QEMU, and Spike. It covers setting up the development environment, writing and debugging bare-metal programs, and implementing advanced features like interrupts, atomic operations, and UART output. The examples are tailored for the RISC-V SoC Lab using the SCL180 PDK, with QEMU's virt machine and a memory-mapped UART at 0x10000000.
Table of Contents

Setting Up the RISC-V Toolchain
Minimal Bare-Metal Hello World
Generating Assembly Code
Converting ELF to Binary/Hex and Disassembling
RV32 Registers and Calling Conventions
Debugging with GDB
Running Bare-Metal Programs in QEMU/Spike
Comparing -O0 vs -O2 Optimizations
Reading the Cycle Counter
Toggling a GPIO Register
Minimal Linker Script
Understanding crt0.S
Enabling Machine-Timer Interrupts (MTIP)
The 'A' (Atomic) Extension
Two-Thread Mutex with LR/SC
Retargeting _write for UART printf
Verifying RV32 Endianness

Setting Up the RISC-V Toolchain
Download and install the xPack RISC-V GNU Toolchain to compile and debug RV32IMAC programs.
wget https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v14.2.0-3/xpack-riscv-none-elf-gcc-14.2.0-3-linux-x64.tar.gz
tar -xvzf xpack-riscv-none-elf-gcc-14.2.0-3-linux-x64.tar.gz
export PATH=$PWD/xpack-riscv-none-elf-gcc-14.2.0-3/bin:$PATH

Verify the toolchain:
riscv-none-elf-gcc --version
riscv-none-elf-objdump --version
riscv-none-elf-gdb --version

Image Placeholder: Screenshot of terminal showing toolchain version output (toolchain_version.png).
Minimal Bare-Metal Hello World
Create a minimal bare-metal program that prints "Hello, World!" to UART (0x10000000).
// hello.c
int main() {
    volatile char *uart = (char *)0x10000000;
    const char *msg = "Hello, World!\n";
    while (*msg) {
        *uart = *msg++;
    }
    return 0;
}

void _start() {
    main();
    while (1);
}

Linker script:
/* linker.ld */
ENTRY(_start)
SECTIONS {
    . = 0x80000000;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}

Compile:
riscv-none-elf-gcc -march=rv32imc -mabi=ilp32 -nostdlib -nostartfiles -T linker.ld -o hello.elf hello.c

Verify:
riscv-none-elf-readelf -h hello.elf

Image Placeholder: Screenshot of hello.elf output in QEMU (hello_output.png).
Generating Assembly Code
Generate assembly for hello.c to inspect the prologue/epilogue of main.
riscv-none-elf-gcc -S -march=rv32imc -mabi=ilp32 -nostdlib -o hello.s hello.c

Example main assembly:
main:
    addi sp,sp,-32    # Prologue: Allocate stack
    sw ra,28(sp)      # Save return address
    sw s0,24(sp)      # Save frame pointer
    addi s0,sp,32     # Set frame pointer
    # Function body
    lw ra,28(sp)      # Epilogue: Restore ra
    lw s0,24(sp)      # Restore s0
    addi sp,sp,32     # Deallocate stack
    ret               # Return

Image Placeholder: Snippet of hello.s in a text editor (assembly_snippet.png).
Converting ELF to Binary/Hex and Disassembling
Convert hello.elf to binary or hex:
riscv-none-elf-objcopy -O binary hello.elf hello.bin
riscv-none-elf-objcopy -O ihex hello.elf hello.hex

Disassemble:
riscv-none-elf-objdump -d hello.elf

Example output:
80000000 <main>:
80000000: 1101          addi sp,sp,-32
80000002: ce06          sw ra,28(sp)
80000004: cc22          sw s0,24(sp)

Columns:

80000000: Memory address
1101: Machine code (hex)
addi sp,sp,-32: Assembly instruction

Image Placeholder: Terminal showing objdump output (disassembly_output.png).
RV32 Registers and Calling Conventions
RV32 has 32 integer registers:



Reg No
ABI Name
Role



x0
zero
Constant zero


x1
ra
Return address


x2
sp
Stack pointer


x3
gp
Global pointer


x4
tp
Thread pointer


x5-7
t0-t2
Temporaries


x8
s0/fp
Saved register/Frame pointer


x9
s1
Saved register


x10-11
a0-a1
Arguments/Return values


x12-17
a2-a7
Arguments


x18-27
s2-s11
Saved registers


x28-31
t3-t6
Temporaries


Image Placeholder: Table of registers (register_table.png).
Debugging with GDB
Debug hello.elf:
riscv-none-elf-gdb hello.elf
(gdb) target sim
(gdb) load
(gdb) break main
(gdb) run
(gdb) step
(gdb) info registers
(gdb) quit

Image Placeholder: GDB session screenshot (gdb_session.png).
Running Bare-Metal Programs in QEMU/Spike
Run in QEMU:
qemu-system-riscv32 -nographic -machine virt -bios none -kernel hello.elf

Run in Spike:
spike --isa=rv32imc hello.elf

Image Placeholder: QEMU terminal output (qemu_output.png).
Comparing -O0 vs -O2 Optimizations
Compile with -O0 and -O2:
riscv-none-elf-gcc -g -O0 -march=rv32imc -mabi=ilp32 -nostdlib -T linker.ld -o hello_O0.elf hello.c startup.s
riscv-none-elf-gcc -g -O2 -march=rv32imc -mabi=ilp32 -nostdlib -T linker.ld -o hello_O2.elf hello.c startup.s

Differences:

-O0: Larger stack frame, stack-based variables, verbose instructions.
-O2: Smaller stack, register-based variables, fewer instructions.

Image Placeholder: Side-by-side assembly comparison (optimization_comparison.png).
Reading the Cycle Counter
Read the cycle counter (CSR 0xC00):
uint32_t read_cycle_counter() {
    uint32_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

Constraints:

"=r" (cycles): Output to a register, stored in cycles.

Image Placeholder: Code snippet with cycle counter output (cycle_counter.png).
Toggling a GPIO Register
Toggle a GPIO at 0x10012000:
#define GPIO 0x10012000
void toggle_gpio() {
    volatile uint32_t *gpio = (uint32_t *)GPIO;
    *gpio ^= 1; // Toggle bit 0
}

Use volatile to prevent optimization.
Image Placeholder: GPIO toggle output (gpio_toggle.png).
Minimal Linker Script
Linker script for .text at 0x00000000, .data at 0x10000000:
ENTRY(_start)
SECTIONS {
    . = 0x00000000;
    .text : { *(.text) }
    . = 0x10000000;
    .data : { *(.data) }
    .bss : { *(.bss) }
}

Image Placeholder: Linker script in editor (linker_script.png).
Understanding crt0.S
crt0.S initializes the bare-metal environment:

Sets stack pointer (sp).
Zeroes .bss.
Copies .data from flash to RAM.
Calls main.

Example:
.section .text.start
.global _start
_start:
    la sp, _stack_top
    call main
    j .

Image Placeholder: crt0.S snippet (crt0_snippet.png).
Enabling Machine-Timer Interrupts (MTIP)
Enable MTIP and handle it:
#define MTIME 0x0200bff8
#define MTIMECMP 0x02004000
void timer_handler() {
    volatile uint64_t *mtimecmp = (uint64_t *)MTIMECMP;
    *mtimecmp += 1000000;
}

Image Placeholder: MTIP output in QEMU (mtip_output.png).
The 'A' (Atomic) Extension
The 'A' extension adds atomic instructions:

LR/SC: lr.w, sc.w for flexible atomic operations.
AMO: amoadd.w, amoswap.w, etc., for read-modify-write.

Image Placeholder: Diagram of LR/SC operation (atomic_diagram.png).
Two-Thread Mutex with LR/SC
Simulate two threads with a mutex:
volatile uint32_t mutex = 0;
int mutex_lock(volatile uint32_t *mutex) {
    uint32_t tmp;
    do {
        asm volatile ("lr.w %0, (%1)" : "=r"(tmp) : "r"(mutex));
        if (tmp != 0) continue;
        asm volatile ("sc.w %0, %2, (%1)" : "=r"(tmp) : "r"(mutex), "r"(1));
    } while (tmp != 0);
    return 0;
}

Image Placeholder: Mutex output (mutex_output.png).
Retargeting _write for UART printf
Redirect printf to UART:
int _write(int fd, const char *buf, unsigned int len) {
    if (fd != 1 && fd != 2) return -1;
    for (unsigned int i = 0; i < len; i++) {
        volatile char *uart = (char *)0x10000000;
        while (!(*(volatile char *)0x10000005 & (1 << 5)));
        *uart = buf[i];
    }
    return len;
}

Image Placeholder: printf output (printf_output.png).
Verifying RV32 Endianness
RV32 is little-endian by default. Verify with a union:
#include <stdio.h>
union endian_test {
    uint32_t value;
    uint8_t bytes[4];
};
int main() {
    union endian_test test = { .value = 0x01020304 };
    printf("Value: 0x%08x\n", test.value);
    for (int i = 0; i < 4; i++) {
        printf("Byte %d: 0x%02x\n", i, test.bytes[i]);
    }
    printf(test.bytes[0] == 0x04 ? "Little-endian\n" : "Big-endian\n");
    return 0;
}

Expected output:
Value: 0x01020304
Byte 0: 0x04
Byte 1: 0x03
Byte 2: 0x02
Byte 3: 0x01
Little-endian

Image Placeholder: Endianness output (endianness_output.png).
Contributing
Feel free to contribute by adding examples or improving documentation. For issues, contact kunalpghosh@vlsisystemdesign.com.
License
MIT License
