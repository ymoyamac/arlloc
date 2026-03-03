#include "list.hpp"

int main() {

    auto node = Node<int>::init(11);

    auto list = LinkedList<int>{};

    list.push_back(1);
    list.push_front(10);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);

    std::println("{}", list.to_string());

    std::println("Size: {}", list.get_size());
    std::println("Node: {}", node->data);

    std::optional<int> data = list.pop_back();

    std::println("Data: {}", data.has_value() ? data.value() : -1);
    std::println("Size: {}", list.get_size());

    std::optional<int> data2 = list.pop_front();

    std::println("Data: {}", data2.has_value() ? data2.value() : -1);
    std::println("Size: {}", list.get_size());

    std::println("{}", list.to_string());

    list.push_front(99);
    std::println("{}", list.to_string());
    
    std::println("Index 0: {}", list.at(0));


    return 0;
}