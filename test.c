#include <stdio.h>

void test();

int main() {
    printf("File:\n");
    test();

    return 0;
}


// nasm -felf64 famine.asm -o famine.o
// gcc -c test.c -o test.o
// gcc test.o famine.o -no-pie -o prog