#pragma once
#include <memory>

template<typename T>
class Node {
public:

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
    Node(T data) : data(data), next(nullptr), prev(nullptr) {}

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

    /**
     * Returns a const reference to the data stored in the node.
     * The `const` on the method guarantees it does not modify the node.
     *
     * @return  Const reference to the node's data.
     */
    const T& get_data(void) const {
        return this->data;
    }

    /**
     * Replaces the data stored in the node with a new value.
     *
     * @param data  The new value to store.
     */
    void set_data(T data) {
        this->data = data;
    }

private:
    /**
     * The value stored in this node.
     * Owned directly by the node (not a pointer).
     */
    T data;

    /**
     * Owning pointer to the next node in the list.
     * When this node is destroyed, the next node is destroyed automatically in cascade.
     *
     *  this                  next
     *  +-------+--------+    +-------+--------+
     *  | data  |  next  +--->| data  |  next  +---> ...
     *  +-------+--------+    +-------+--------+
     */
    std::unique_ptr<Node<T>> next;

    /**
     * Non-owning pointer to the previous node in the list.
     * Raw pointer because `prev` does not own the node it points to,
     * it only references it to allow backwards traversal.
     *
     *  prev                  this
     *  +-------+--------+    +-------+--------+
     *  | data  |  next  +<---+ prev  | data   |
     *  +-------+--------+    +-------+--------+
     */
    Node<T>* prev;
};