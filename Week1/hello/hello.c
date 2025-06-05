// hello.c
int main() {
    volatile char *uart = (char *)0x10000000;
    const char *msg = "Hello, World!\n";
    while (*msg) {
        *uart = *msg++;
    }
    return 0;
}

// Define _start as entry point
void _start() {
    main();
    while (1); // hang after main
}

