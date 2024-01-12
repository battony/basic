#include "basic/coroutine/Coroutine.hxx"

#include <iostream>

auto main() -> int {
    basic::coroutine::Coroutine c1(
            []() -> void {
                std::cout << "1" << std::endl;
                basic::coroutine::Coroutine::SwitchBack();
                std::cout << "2" << std::endl;
            }, 1024 * 1024);
    basic::coroutine::Coroutine c2(
            [&]() -> void {
                std::cout << "3" << std::endl;
                basic::coroutine::Coroutine::Switch(c1);
                std::cout << "4" << std::endl;
                basic::coroutine::Coroutine::Switch(c1);
                std::cout << "5" << std::endl;
            }, 1024 * 1024);
    basic::coroutine::Coroutine::Switch(c2);
    return 0;
}
