#include <stdint.h>

// Function to read the 32-bit cycle counter from CSR 0xC00
uint32_t read_cycle_counter(void) {
    uint32_t cycles;
    __asm__ volatile (
        "rdcycle %0"
        : "=r" (cycles) // Output constraint
        :               // No input constraints
        :               // No clobbered registers
    );
    return cycles;
}
