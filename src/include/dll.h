#ifndef DLL_H
#define DLL_H

#include <stdlib.h>
#include <stdio.h>
#include "lib.h"

#define DEFAULT_CAPACITY 10

#define push_back(list, val) __push_back(list,      \
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

#define push_front(list, val) __push_front(list,    \
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
} DataTypes;

typedef struct node_s Node;

struct node_s
{
    void* data;
    DataTypes type;
    Node* next;
    Node* prev;
};

Node* make_node(void* data, DataTypes type);

typedef struct dll_s Dll;

struct dll_s
{
    size_t size;
    size_t capacity;
    Node* head;
    Node* tail;
};

Dll* make_list();
//Dll* make_list(void* arr);
void fmt(Dll* self);
void* get_data_at(Dll* self, size_t index);
void __push_front(Dll* self, void* data, DataTypes type);
void __push_back(Dll* self, void* data, DataTypes type);
void* pop_front(Dll* self);
void* pop_back(Dll* self);


#endif