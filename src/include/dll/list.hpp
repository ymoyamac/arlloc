#pragma once

#include <sstream>
#include <optional>
#include <string>
#include "dll/node.hpp"

template<typename T>
class LinkedList {
public:

    LinkedList() : head(nullptr), tail(nullptr), size(0) {}
    ~LinkedList() = default;

    std::size_t& get_size() {
        return this->size;
    }

    /**
     * O(1) - A new node is created with the data to be stored.
     *
     *  Head                          Tail                          NewNode
     *  +-------+--------+    +-------+--------+    +-------+--------+
     *  | data  |  next  +<-->| data  |  next  +<-->| data  |  next  +--> NULL
     *  +-------+--------+    +-------+--------+    +-------+--------+
     */
    void push_back(T data) {
        auto new_node = Node<T>::init(std::move(data));
        Node<T>* raw = new_node.get();
        if (this->tail == nullptr) {
            this->head = std::move(new_node);
        } else {
            new_node->prev = tail;
            this->tail->next = std::move(new_node);
        }
        this->tail = raw;
        this->size++;
    }

    void push_front(T data) {
        auto new_node = Node<T>::init(std::move(data));
        Node<T>* raw = new_node.get();

        if (this->head == nullptr) {
            this->tail = raw;
            this->head = std::move(new_node);
        } else {
            Node<T>* old_head = this->head.get();
            new_node->next = std::move(this->head);
            old_head->prev = raw;
            this->head = std::move(new_node);
        }
        this->size++;
    }

    std::optional<T> pop_back() {
        if (this->tail == nullptr) {
            return {};
        }

        std::optional<T> data = this->tail->data;
        if (this->tail->prev == nullptr) {
            this->head = nullptr;
            this->tail = nullptr;
        } else {
            this->tail = this->tail->prev;
            this->tail->next = nullptr;
        }
        this->size--;
        return std::optional{data};
    }

    std::optional<T> pop_front() {
        if (this->head == nullptr) {
            return {};
        }

        std::optional<T> data = this->head->data;
        if (this->head->next == nullptr) {
            this->head = nullptr;
            this->tail = nullptr;
        } else {
            this->head = std::move(this->head->next);
            this->head->prev = nullptr;
        }
        this->size--;
        return std::optional{data};
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "List { ";
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

private:
    std::unique_ptr<Node<T>> head;
    Node<T>* tail = nullptr;
    std::size_t size = 0;


};