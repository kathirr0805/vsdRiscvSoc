#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define UART_TX 0x10000000    // UART transmit register (QEMU virt)
#define UART_READY 0x10000005 // UART status register (bit 5 = TX ready)

// UART write function
void uart_putc(char c) {
    volatile char* uart_tx = (volatile char*)UART_TX;
    volatile char* uart_ready = (volatile char*)UART_READY;
    while (!(*uart_ready & (1 << 5))); // Wait for TXFIFO empty
    *uart_tx = c;
}

// Retarget _write for printf
int _write(int fd, const char *buf, unsigned int len) {
    if (fd != 1 && fd != 2) return -1; // Only handle stdout/stderr
    for (unsigned int i = 0; i < len; i++) {
        uart_putc(buf[i]);
    }
    return len;
}

// Minimal system call stubs
int _close(int fd) {
    errno = EBADF;
    return -1;
}

off_t _lseek(int fd, off_t offset, int whence) {
    errno = ESPIPE;
    return -1;
}

int _read(int fd, char *buf, unsigned int len) {
    errno = EBADF;
    return -1;
}

int _fstat(int fd, struct stat *buf) {
    if (fd == 1 || fd == 2) {
        buf->st_mode = S_IFCHR; // Character device for stdout/stderr
        return 0;
    }
    errno = EBADF;
    return -1;
}

int _isatty(int fd) {
    if (fd == 1 || fd == 2) return 1; // stdout/stderr are terminals
    errno = EBADF;
    return 0;
}

void *_sbrk(int incr) {
    extern char _end; // Defined in linker.ld
    static char *heap_end = 0;
    char *prev_heap_end;

    if (heap_end == 0) heap_end = &_end;

    prev_heap_end = heap_end;
    heap_end += incr;

    // Simple check to avoid heap overflow (adjust as needed)
    if (heap_end > (char*)0x81010000) { // Limit heap to 64KB
        errno = ENOMEM;
        return (void*)-1;
    }
    return prev_heap_end;
}

void _exit(int status) {
    while (1); // Hang on exit
}

int _kill(pid_t pid, int sig) {
    errno = EINVAL;
    return -1;
}

pid_t _getpid(void) {
    return 1; // Single process
}

int main() {
    uart_putc('A'); // Direct UART test
    printf("Hello, RISC-V! Counter: %d\n", 42); // Test printf
    while (1) {
        uart_putc('.');
        for (volatile int i = 0; i < 100000; i++);
    }
    return 0;
}
