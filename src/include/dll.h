#ifndef DLL_H
#define DLL_H

#include <stdlib.h>
#include <stdio.h>
#include "lib.h"

#define DEFAULT_CAPACITY 10

#define push_back_auto(list, val) push_back(list,   \
    _Generic((val),                                 \
        int:    &(val),                             \
        float:  &(val),                             \
        char:   &(val),                             \
        char*:  (val)                               \
    ),                                              \
    _Generic((val),                                 \
        int:    TYPE_INT,                           \
        float:  TYPE_FLOAT,                         \
        char:   TYPE_CHAR,                          \
        char*:  TYPE_STRING                         \
    ))

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} data_type;

typedef struct node_s node;

struct node_s
{
    void* data;
    data_type type;
    node* next;
    node* prev;
};

node* make_node(void* data, data_type type);

typedef struct dll_s dll;

struct dll_s
{
    size_t size;
    size_t capacity;
    node* head;
    node* tail;
};

dll* make_list();
void fmt(dll* self);
void* get_data_at(dll* self, size_t index);
void push_front(dll* self, void* data);
void push_back(dll* self, void* data, data_type type);
void* pop_front(dll* self);
void* pop_back(dll* self);


#endif