#include "arlloc.hpp"

int main() {
    Arlloc arlloc = Arlloc{};

    int* num = (int*)arlloc.alloc(sizeof(int) * 10);
    *num = 10;
    printf("Num 1: %d\n", *num);

    int* num2 = (int*)arlloc.alloc(sizeof(int) * 10);
    *num2 = 99;
    printf("Num 2: %d\n", *num2);

    int* num3 = (int*)arlloc.alloc(sizeof(int) * 10);
    *num3 = 101;
    printf("Num 3: %d\n", *num3);

    int* num4 = (int*)arlloc.alloc(sizeof(int) * 10);
    *num4 = 10;
    printf("Num 4: %d\n", *num4);

    printf("Dealloc num 3\n");
    arlloc.dealloc(num3);
    printf("Dealloc num 2\n");
    arlloc.dealloc(num2);

    arlloc.dealloc(num);
    arlloc.dealloc(num4);

    return 0;
}