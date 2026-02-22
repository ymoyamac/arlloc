#include "dll.h"

int main() {

    dll* list = make_list();

    int numbers[] = {1, 2, 3, 4, 5, 6};
    char* words[] = {"hello", "world"};

    for (size_t i = 0; i < len(numbers); i++) {
        push_back_auto(list, numbers[i]);
    }

    for (size_t i = 0; i < len(words); i++)
    {
        push_back_auto(list, words[i]);
    }

    fmt(list);

    int* number = (int*)get_data_at(list, 2);
    printf("Number: %d\n", *number);

    char* word = (char*)get_data_at(list, 6);
    printf("Word: %s\n", word);

    return 0;
}