#include "arlloc.hpp"

int main() {

    Arlloc arlloc = Arlloc{};

    /**
     * Allocate 10 bytes and write exactly 10 characters.
     * chars[10] would be out of bounds and is removed.
     */
    char* chars = (char*)arlloc.alloc(10);

    chars[0] = 'a';
    chars[1] = 'b';
    chars[2] = 'c';
    chars[3] = '1';
    chars[4] = '2';
    chars[5] = '3';
    chars[6] = 'x';
    chars[7] = 'y';
    chars[8] = 'z';
    chars[9] = '0';

    for (int i = 0; i < 10; i++) {
        printf("CharsAdrr: \x1B[33m%p\033[0m, Value: %c\n", chars + i, chars[i]);
    }

    /** Second allocation reuses the free block from the first region via block splitting. */
    int* num = (int*)arlloc.alloc(sizeof(int) * 10);
    *num = 10;
    printf("Num: %d\n", *num);

    /** Deallocate the char buffer, marking it as free for future allocations. */
    arlloc.dealloc(chars);
    arlloc.dealloc(num);

    return 0;
}