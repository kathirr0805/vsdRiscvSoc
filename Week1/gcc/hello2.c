#define UART_TX 0x10000000 // UART transmit register for QEMU virt machine
#define UART_READY 0x10000005 // UART status register (bit 5 = TX ready)

void uart_putc(char c) {
    volatile char* uart_tx = (volatile char*)UART_TX;
    volatile char* uart_ready = (volatile char*)UART_READY;
    while (!(*uart_ready & (1 << 5))); // Wait for TXFIFO empty (bit 5)
    *uart_tx = c; // Write character to UART
}

void uart_puts(const char* s) {
    while (*s) {
        uart_putc(*s++);
    }
}

int main() {
    volatile int x = 42;
    x = x + 1;
    uart_puts("Value of x: ");
    // Convert x to string (simple decimal conversion)
    if (x == 0) {
        uart_putc('0');
    } else {
        char buf[10];
        int i = 0;
        while (x > 0) {
            buf[i++] = '0' + (x % 10);
            x /= 10;
        }
        while (i > 0) {
            uart_putc(buf[--i]);
        }
    }
    uart_puts("\n");
    return x;
}
