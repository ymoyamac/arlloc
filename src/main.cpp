#include "arlloc.hpp"

int main() {

    auto node = Node<int>::init(11);

    std::println("Node: {}", node->get_data());

    return 0;
}