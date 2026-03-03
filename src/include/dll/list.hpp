#pragma once

#include <sstream>
#include <optional>
#include <string>
#include "dll/node.hpp"

template<typename T>
class LinkedList {
public:

    /**
     * Constructs an empty linked list.
     * head and tail are set to nullptr, size is set to 0.
     *
     *  +------+------+------+
     *  | size | head | tail |
     *  +------+------+------+
     *  |  0   | NULL | NULL |
     *  +------+------+------+
     */
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    /**
     * Destroys all nodes in the list.
     * `head` unique_ptr destroys the chain automatically from head to tail.
     */
    ~LinkedList() = default;

    /**
     * Returns a reference to the number of nodes in the list.
     *
     * @return  Reference to the size of the list.
     */
    std::size_t& get_size() {
        return this->size;
    }

    /**
     * O(1) - Inserts a new node at the end of the list.
     *
     * @param data  The value to store in the new node.
     */
    void push_back(T data) {
        auto new_node = Node<T>::init(std::move(data));
        Node<T>* raw = new_node.get();

        if (this->tail == nullptr) {
            /**
             * Case 1 - List is empty: head owns the node, tail points to it.
             *
             *  Head, Tail
             *  +-------+--------+--------+
             *  | data  |  next  |  prev  |
             *  +-------+--------+--------+
             *              NULL    NULL
             */
            this->head = std::move(new_node);
        } else {
            /**
             * Case 2 - List has elements.
             *
             *  Step 1: new_node->prev points to tail, tail->next owns new_node.
             *
             *  Head                          Tail                          NewNode
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *  | data  |  next  +<-->| data  |  next  +<-->| data  |  next  +--> NULL
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *
             *  Step 2: tail is updated to new_node.
             *
             *  Head                                                        Tail
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *  | data  |  next  +<-->| data  |  next  +<-->| data  |  next  +--> NULL
             *  +-------+--------+    +-------+--------+    +-------+--------+
             */
            new_node->prev = tail;
            this->tail->next = std::move(new_node);
        }
        this->tail = raw;
        this->size++;
    }

    /**
     * O(1) - Inserts a new node at the beginning of the list.
     *
     * @param data  The value to store in the new node.
     */
    void push_front(T data) {
        auto new_node = Node<T>::init(std::move(data));
        Node<T>* raw = new_node.get();

        if (this->head == nullptr) {
            /**
             * Case 1 - List is empty: head owns the node, tail points to it.
             *
             *  Head, Tail
             *  +-------+--------+--------+
             *  | data  |  next  |  prev  |
             *  +-------+--------+--------+
             *              NULL    NULL
             */
            this->tail = raw;
            this->head = std::move(new_node);
        } else {
            /**
             * Case 2 - List has elements.
             *
             *  Step 1: new_node->next owns old head, old_head->prev points back to new_node.
             *          Saves raw pointer before move to update prev after.
             *
             *  NewNode               OldHead
             *  +-------+--------+    +-------+--------+
             *  | data  |  next  +--->| data  |  next  +--> ...
             *  +-------+--------+    +-------+--------+
             *              <----prev----
             *
             *  Step 2: head is updated to new_node.
             *
             *  Head
             *  +-------+--------+    +-------+--------+
             *  | data  |  next  +--->| data  |  next  +--> ...
             *  +-------+--------+    +-------+--------+
             *              <----prev----
             */
            Node<T>* old_head = this->head.get();
            new_node->next = std::move(this->head);
            old_head->prev = raw;
            this->head = std::move(new_node);
        }
        this->size++;
    }

    /**
     * O(1) - Removes the last node from the list and returns its data.
     * Returns an empty `std::optional` if the list is empty.
     *
     * @return  The data of the removed node, or `std::nullopt` if empty.
     */
    std::optional<T> pop_back() {
        if (this->tail == nullptr) {
            return {};
        }

        std::optional<T> data = this->tail->data;
        if (this->tail->prev == nullptr) {
            /** List had only one node, reset to empty state. */
            this->head = nullptr;
            this->tail = nullptr;
        } else {
            /**
             * Step 1: tail moves to the previous node.
             *
             *  Head                          Tail
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *  | data  |  next  +<-->| data  |  next  +<-->| data  |  next  +--> NULL
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *
             * Step 2: tail->next is set to nullptr.
             *         unique_ptr destroys the old tail node automatically.
             *
             *  Head                  Tail
             *  +-------+--------+    +-------+--------+
             *  | data  |  next  +<-->| data  |  next  +--> NULL     old tail: destroyed ✓
             *  +-------+--------+    +-------+--------+
             */
            this->tail = this->tail->prev;
            this->tail->next = nullptr;
        }
        this->size--;
        return std::optional{data};
    }

    /**
     * O(1) - Removes the first node from the list and returns its data.
     * Returns an empty `std::optional` if the list is empty.
     *
     * @return  The data of the removed node, or `std::nullopt` if empty.
     */
    std::optional<T> pop_front() {
        if (this->head == nullptr) {
            return {};
        }

        std::optional<T> data = this->head->data;
        if (this->head->next == nullptr) {
            /** List had only one node, reset to empty state. */
            this->head = nullptr;
            this->tail = nullptr;
        } else {
            /**
             * Step 1: head moves ownership to head->next.
             *         unique_ptr destroys the old head node automatically.
             *
             *  OldHead (destroyed ✓)
             *  Head
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *  | data  |  next  +<-->| data  |  next  +<-->| data  |  next  +--> NULL
             *  +-------+--------+    +-------+--------+    +-------+--------+
             *
             * Step 2: new head->prev is set to nullptr.
             *
             *  Head
             *  +-------+--------+    +-------+--------+
             *  | data  |  next  +<-->| data  |  next  +--> NULL
             *  +-------+--------+    +-------+--------+
             *   prev=NULL
             */
            this->head = std::move(this->head->next);
            this->head->prev = nullptr;
        }
        this->size--;
        return std::optional{data};
    }

    /**
     * Builds and returns a string representation of the list.
     * Requires `T` to support `operator<<`.
     *
     * @return  A string in the format "List { 1, 2, 3 }".
     */
    std::string to_string() const {
        std::ostringstream oss;
        oss << "List { ";
        /** Traverses the list from head to tail using raw pointers. */
        Node<T>* iter = this->head.get();
        while (iter != nullptr) {
            oss << iter->data;
            if (iter->next != nullptr) {
                oss << ", ";
            }
            iter = iter->next.get();
        }
        oss << " }";
        return oss.str();
    }

    /**
     * Prints the list to stdout using `to_string()`.
     * Output example: List { 1, 2, 3 }
     */
    void print() const {
        std::cout << this->to_string() << std::endl;
    }

    /**
     * O(n) - access by index.
     * Optimised: search from head or tail depending on which side is closer.
     */
    T& at(std::size_t index) {
        if (index >= this->size) throw std::out_of_range("Index out of range");
        Node<T>* iter;
        if (index < this->size / 2) {
            iter = this->head.get();
            for (std::size_t i = 0; i < index; i++)
                iter = iter->next.get();
        } else {
            iter = this->tail;
            for (std::size_t i = this->size - 1; i > index; i--)
                iter = iter->prev;
        }
        return iter->data;
    }


private:
    /**
     * Owning pointer to the first node.
     * Destroying head destroys the entire chain automatically.
     */
    std::unique_ptr<Node<T>> head;

    /**
     * Non-owning pointer to the last node.
     * Allows O(1) push_back and pop_back without traversing the list.
     */
    Node<T>* tail = nullptr;

    /**
     * Number of nodes currently in the list.
     */
    std::size_t size = 0;
};