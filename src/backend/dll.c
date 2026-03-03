#include "dll.h"

/**
 * Creates and initializes a new node with the given data and type.
 * If `malloc` cannot allocate memory, it returns `NULL`.
 *
 * @param data  Pointer to the data to store in the node.
 * @param type  The data type of the stored value (`DataTypes` enum).
 * @return      Pointer to the new node, or `NULL` if allocation fails.
 */
Node* make_node(void* data, DataTypes type) {
    Node* node_ptr = (Node*)malloc(sizeof(Node));
    if (node_ptr == NULL) {
        printf("Error allocating node...\n");
        return NULL;
    }

    /**
     * The fields of the node are initialized with the given data and type.
     * next and prev are set to NULL as the node is not yet part of a list.
     *
     *  +-------+-------+--------+--------+
     *  | data  | type  |  next  |  prev  |
     *  +-------+-------+--------+--------+
     *      |               |        |
     *      v             NULL      NULL
     *   (void*)
     */
    node_ptr->data = data;
    node_ptr->type = type;
    node_ptr->next = NULL;
    node_ptr->prev = NULL;

    return node_ptr;
}

/**
 * Creates and initializes an empty doubly linked list.
 * If `malloc` cannot allocate memory, it returns `NULL`.
 *
 * @return  Pointer to the new list, or `NULL` if allocation fails.
 */
Dll* make_list() {
    Dll* list = (Dll*)malloc(sizeof(Dll));
    if (list == NULL) {
        printf("Error to create list...\n");
        return NULL;
    }

    /**
     * The list fields are initialized.
     * head and tail are NULL because the list is empty.
     *
     *  dll
     *  +----------+------+------+------+
     *  | capacity | size | head | tail |
     *  +----------+------+------+------+
     *  |    10    |   0  | NULL | NULL |
     *  +----------+------+------+------+
     */
    list->capacity = DEFAULT_CAPACITY;
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

/**
 * Free all elements of the list.
 * Does nothing if `self` is `NULL`.
 *
 * @param self  Pointer to the list to print.
 */
void drop(Dll* self) {
    Node* iter = self->tail;
    while (iter) {
        /**
         * Free elements: Poping the last element of the list.
         *
         *  Step 1: 
         * 
         *  Head                          Tail                              (free)
         *  +-------+-------+--------+    +-------+-------+--------+        +-------+-------+--------+
         *  | prev  | data  |  next  +<-->| prev  | data  |  next  + >NULL  | prev  | data  |  next  +---> NULL
         *  +-------+-------+--------+    +-------+-------+--------+        +-------+-------+--------+
         *                                                     ^                |
         *                                                     |                |
         *                                                     +--------X-------+
         * 
         */
        free(iter->next);
        iter = iter->prev;
    }
    free(self->tail->next);
    free(self->head);
    free(self);
}

/**
 * Prints all elements of the list to stdout.
 * Each element is formatted according to its `DataTypes`.
 * Does nothing if `self` is `NULL`.
 *
 * @param self  Pointer to the list to print.
 */
void fmt(Dll* self) {

    if (self == NULL) {
        printf("Nothing to do...");
        return;
    }

    if (self->head == NULL && self->tail == NULL) {
        printf("List {}\n");
        return;
    }

    Node* iter = self->head;
    printf("List {");
    while (iter) {
        /**
         * The list is traversed from head to tail.
         * Each node is printed according to its type.
         *
         *  Head                                        Tail
         *  +-------+-------+--------+    +-------+-------+--------+
         *  | data  | type  |  next  +--->| data  | type  |  next  +---> NULL
         *  +-------+-------+--------+    +-------+-------+--------+
         *  TYPE_INT                       TYPE_STRING
         *  printf("%i", ...)              printf("%s", ...)
         */
        switch (iter->type) {
            case TYPE_INT:    printf("%i",  *(int*)iter->data);   break;
            case TYPE_FLOAT:  printf("%f",  *(float*)iter->data); break;
            case TYPE_CHAR:   printf("%c",  *(char*)iter->data);  break;
            case TYPE_STRING: printf("%s",  (char*)iter->data);   break;
            default: printf("%s", (char*)iter->data);   break;
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

/**
 * Returns a pointer to the data stored at the given index.
 * Returns `NULL` if the list is empty, `self` is `NULL`, or the index is out of bounds.
 *
 * @param self   Pointer to the list.
 * @param index  Zero-based position to retrieve.
 * @return       Pointer to the data at `index`, or `NULL` if invalid.
 */
void* get_data_at(Dll* self, size_t index) {
    if (self == NULL || self->size == 0 || index > self->size) {
        return NULL;
    }

    Node* iter = self->head;
    /**
     * The list is traversed until the given index is reached.
     *
     *  Head                                                      Iter
     *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
     *  | data  | type  |  next  +--->| data  | type  |  next  +--->| data  | type  |  next  +---> ...
     *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
     *  index=0                       index=1                       index=2
     *                                                                   |
     *                                                               return iter->data
     */
    for(size_t i = 0; i < index; i++) {
        iter = iter->next;
    }
    return iter->data;
}

/**
 * Inserts a new node at the end of the list.
 *
 * @param self  Pointer to the list.
 * @param data  Pointer to the data to store.
 * @param type  The data type of the stored value (`DataTypes` enum).
 */
void push_back(Dll* self, void* data, DataTypes type) {

    if (self == NULL) {
        return;
    }

    /** A new node is created with the data to be stored. */
    Node* new_node = make_node(data, type);
    if (new_node == NULL) {
        return;
    }

    /**
     * Case 1 - List is empty: head and tail point to the new node.
     *
     *  Head, Tail
     *  +-------+-------+--------+--------+
     *  | prev  | data  |  next  |  type  |
     *  +-------+-------+--------+--------+
     *   NULL                      NULL
     */
    if (self->head == NULL && self->tail == NULL) {
        printf("List is empty...\n");
        self->head = new_node;
        self->tail = new_node;
    } else {
        /**
         * Case 2 - List has elements: the new node is linked after the current tail.
         *
         *  Step 1: tail->next points to new_node, new_node->prev points back to tail.
         *
         *  Head                          Tail                          NewNode
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         *  | prev  | data  |  next  +<-->| prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         *
         *  Step 2: tail is updated to new_node.
         *
         *  Head                                                        Tail
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         *  | prev  | data  |  next  +<-->| prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         */
        printf("Allocating new item...\n");
        self->tail->next = new_node;
        new_node->prev = self->tail;
        self->tail = new_node;
    }
    self->size++;
}

/**
 * Inserts a new node at the being of the list.
 *
 * @param self  Pointer to the list.
 * @param data  Pointer to the data to store.
 * @param type  The data type of the stored value (`DataTypes` enum).
 */
void push_front(Dll* self, void* data, DataTypes type) {

    if (self == NULL) {
        return;
    }

    Node* new_node = make_node(data, type);

    if (new_node == NULL) {
        return;
    }

    /**
     * Case 1 - List is empty: head and tail point to the new node.
     *
     *  Head, Tail
     *  +-------+-------+--------+--------+
     *  | prev  | data  |  next  |  type  |
     *  +-------+-------+--------+--------+
     *   NULL                      NULL
     */
    if (self->head == NULL && self->tail == NULL) {
        printf("Empty list...\n");
        self->head = new_node;
        self->tail = new_node;
    } else {
        /**
         * Case 2 - List has elements: the new node is linked after the current tail.
         *
         *  Step 1: new_node->next points to the head of the list.
         *
         *  NewNode                       Head                          Tail
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         *  | prev  | data  |  next  +<-->| prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         *
         *  Step 2: Head is updated to new_node.
         *
         *  Head                                                        Tail
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         *  | prev  | data  |  next  +<-->| prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
         *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
         */
        printf("Allocating new item...\n");
        new_node->next = self->head;
        self->head->prev = new_node;
        self->head = new_node;
    }
    self->size++;
    
}

/**
 * Pop the last element of the list.
 *
 * @param   self  Pointer to the list.
 * @return  void* data stored in the last node.
 */
void* pop_back(Dll* self) {

    if (self == NULL) {
        return NULL;
    }

    /**
     * If the list is empty. Nothing to do.
     */
    if (self->head == NULL && self->tail == NULL) {
        printf("Empty list...");
        return NULL;
    }

    printf("Poping...\n");
    /**
     * Case 2 - List has elements: Poping the last element of the list.
     *
     *  Step 1: points the tail of the list to the new_tail.
     *
     *  Head                          NewTail                       Tail
     *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
     *  | prev  | data  |  next  +<-->| prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
     *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
     *
     *  Step 2: new_tail->next points to NULL.
     *
     *  Head                          Tail                              (free)
     *  +-------+-------+--------+    +-------+-------+--------+        +-------+-------+--------+
     *  | prev  | data  |  next  +<-->| prev  | data  |  next  + >NULL  | prev  | data  |  next  +---> NULL
     *  +-------+-------+--------+    +-------+-------+--------+        +-------+-------+--------+
     *                                                     |                ^
     *                                                     |                |
     *                                                     +--------X-------+
     *  Step 3:  Points tail to the new_tail.
     * 
     *  Head                          Tail                    
     *  +-------+-------+--------+    +-------+-------+--------+
     *  | prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
     *  +-------+-------+--------+    +-------+-------+--------+
     */
    Node* new_tail = self->tail->prev;
    free(self->tail);
    new_tail->next = NULL;
    self->tail = new_tail;
    void* data = new_tail->data;
    return data;
}

/**
 * Pop the first element of the list.
 * 
 * @param   self Pointer to the list.
 * @return  void* to the data of the first node.
 */
void* pop_front(Dll* self) {
    if (self == NULL) {
        return NULL;
    }

    /**
     * If the list is empty. Nothing to do.
     */
    if (self->head == NULL && self->tail == NULL) {
        printf("Empty list...\n");
        return NULL;
    }

    printf("Poping first element of the list...\n");
    /**
     * Case 2 - List has elements.
     *
     *  Step 1: new_head points to the next node of head.
     * 
     *  OldHead
     *  Head                          NewHead                       Tail
     *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
     *  | prev  | data  |  next  +<-->| prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
     *  +-------+-------+--------+    +-------+-------+--------+    +-------+-------+--------+
     *
     *  Step 2: Head is updated to new_head.
     *
     *  Head                          Tail
     *  +-------+-------+--------+    +-------+-------+--------+
     *  | prev  | data  |  next  +<-->| prev  | data  |  next  +---> NULL
     *  +-------+-------+--------+    +-------+-------+--------+
     */
    Node* new_head = self->head->next;
    new_head->prev = NULL;
    void* data = self->head->data;
    free(self->head);
    self->head = new_head;
    return data;
}