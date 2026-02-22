#include "dll.h"


node* make_node(void* data, data_type type) {
    node* node_ptr = (node*)malloc(sizeof(node));
    if (node_ptr == NULL) {
        printf("Error allocating node...\n");
        return NULL;
    }

    node_ptr->data = data;
    node_ptr->type = type;
    node_ptr->next = NULL;
    node_ptr->prev = NULL;

    return node_ptr;
}

dll* make_list() {
    dll* list = (dll*)malloc(sizeof(dll));
    if (list == NULL) {
        printf("Error to create list...\n");
        return NULL;
    }

    list->capacity = DEFAULT_CAPACITY;
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void fmt(dll* self) {

    if (self == NULL) {
        return;
    }

    node* iter = self->head;
    printf("List {");
    while (iter) {
        switch (iter->type) {
            case TYPE_INT:    printf("%i",  *(int*)iter->data);   break;
            case TYPE_FLOAT:  printf("%f",  *(float*)iter->data); break;
            case TYPE_CHAR:   printf("%c",  *(char*)iter->data);  break;
            case TYPE_STRING: printf("%s",  (char*)iter->data);   break;
        }
        if (iter->next != NULL) {
            printf(", ");
        } else {
            printf("}");
        }
        iter = iter->next;
    }
    printf("\n");
}

void* get_data_at(dll* self, size_t index) {
    if (self == NULL || self->size == 0 || index > self->size) {
        return NULL;
    }

    node* iter = self->head;
    for(size_t i = 0; i < index; i++) {
        iter = iter->next;
    }
    return iter->data;
    
}

void push_back(dll* self, void* data, data_type type) {
    node* new_node = make_node(data, type);
    if (new_node == NULL) {
        return;
    }

    if (self->head == NULL || self->tail == NULL) {
        printf("List is empty...\n");
        self->head = new_node;
        self->tail = new_node;
        self->size++;
    } else {
        printf("Allocating new item...\n");
        self->tail->next = new_node;
        new_node->prev = self->tail;
        self->tail = new_node;
        self->size++;
    }




}
