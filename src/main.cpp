#include "arlloc.hpp"

int main() {

    Region* region = Region::init();
    char* chars = (char*)region->alloc(10);

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
    chars[10] = '0';

    for (int i = 0; i < 10; i++)
    {
        printf("CharsAdrr: %p, Value: %c\n", chars + i, chars[i]);

    }
    

    Region::drop(region);
    return 0;
}