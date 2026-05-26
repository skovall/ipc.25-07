#include <gtest/gtest.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
    // Устанавливаем UTF-8 для консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Включаем поддержку ANSI escape-последовательностей
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    ::testing::InitGoogleTest(&argc, argv);
    
    int result = RUN_ALL_TESTS();
    
#ifdef WAIT_ON_EXIT
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
#endif
    
    return result;
}
