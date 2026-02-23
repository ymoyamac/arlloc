#include "dll.h"

int main() {

    Dll* list = make_list();

    int numbers[] = {1, 2, 3, 4, 5, 6};
    char* words[] = {"hello", "world"};

    for (size_t i = 0; i < len(numbers); i++) {
        push_back(list, numbers[i]);
    }

    for (size_t i = 0; i < len(words); i++)
    {
        push_back(list, words[i]);
    }

    int num = 99;
    push_front(list, num);
    push_front(list, num);
    push_back(list, num);

    fmt(list);

    int* number = (int*)get_data_at(list, 2);
    printf("Number: %d\n", *number);

    char* word = (char*)get_data_at(list, 6);
    printf("Word: %s\n", word);


    int tail = *(int*) pop_back(list);
    printf("Tail: %d\n", tail);

    fmt(list);

    pop_front(list);
    pop_front(list);
    int head = *(int*) pop_front(list);
    printf("Head: %d\n", head);

    fmt(list);

    drop(list);

    fmt(list);


    return 0;
}