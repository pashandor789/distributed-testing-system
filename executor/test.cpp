#include <iostream> 

#include <fstream>
#include <unistd.h>

#include <array>

int main() {
    std::cout << "hello, i will sleep! my pid is: " << getpid() << std::endl;
    std::cout << "start sleeping..." << std::endl;

    std::array<int, 1'000'000> arr;

    size_t i = 0;
    for (auto& el: arr) {
        el = ++i;
    }

    std::cout << "end sleeping..." << std::endl;
    std::cout << "i am alive!" << std::endl;
}