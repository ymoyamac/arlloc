#pragma once
#include <memory>

template<typename T>
class Node {
public:
    T data;
    std::unique_ptr<Node<T>> next;
    Node<T>* prev = nullptr;

    /**
     * Constructs a new node with the given data.
     * next and prev are set to nullptr as the node is not yet part of a list.
     *
     *  +-------+--------+--------+
     *  | data  |  next  |  prev  |
     *  +-------+--------+--------+
     *      |       |        |
     *      v     nullptr  nullptr
     *     (T)
     */
    explicit Node(T data) : data(std::move(data)), next(nullptr), prev(nullptr) {}

    /**
     * Factory method. Creates a new node on the heap and returns a `unique_ptr` to it.
     * Preferred over the constructor directly when the node needs to live on the heap.
     *
     * @param data  The value to store in the node.
     * @return      A `unique_ptr` owning the new node.
     *
     *  STACK                        HEAP
     *  +------------------+         +-------+--------+--------+
     *  | unique_ptr<Node> +-------> | data  |  next  |  prev  |
     *  +------------------+         +-------+--------+--------+
     */
    static std::unique_ptr<Node<T>> init(T data) {
        return std::make_unique<Node<T>>(data);
    }

    
};