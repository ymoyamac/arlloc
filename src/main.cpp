#include "arlloc.hpp"

int main() {

    Region* region = Region::init();
    std::println("Region size: {}", Region::total_region_size());

    return 0;
}