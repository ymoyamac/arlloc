#include "list.hpp"
#include "lib.hpp"

int main() {

    auto node = Node<int>::init(11);

    auto list = LinkedList<int>{};

    list.push_back(1);
    list.push_front(10);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);

    Logger::info("%s", list.to_string());

    Logger::info("Size: %d", list.get_size());
    Logger::info("Node: %d", node->data);

    std::optional<int> data = list.pop_back();

    Logger::info("Data: %d", data.has_value() ? data.value() : -1);
    Logger::info("Size: %d", list.get_size());

    std::optional<int> data2 = list.pop_front();

    Logger::info("Data: %d", data2.has_value() ? data2.value() : -1);
    Logger::info("Size: %d", list.get_size());

    Logger::info("%s", list.to_string());

    list.push_front(99);
    Logger::info("%s", list.to_string());
    
    Logger::info("Index 0: %d", list.at(0));


    return 0;
}