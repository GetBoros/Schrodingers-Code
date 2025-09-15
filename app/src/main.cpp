#include <iostream>
#include <lib.hpp>
#include <lib_dynamic.hpp>

int main()
{
    Func();
    Func_From_Lib_Dynamic();

    std::cout << "Hello, World!" << std::endl;

    // ждём пока пользователь нажмёт Enter
    // std::cout << "Press enter to continued!" << std::endl;
    // std::cin.get();

    return 0;
}