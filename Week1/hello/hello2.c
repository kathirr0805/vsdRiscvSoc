int main() {
    volatile int x = 42;
    x = x + 1;
    return x;
}
