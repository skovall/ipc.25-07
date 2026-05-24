//проверка на повторный запуск
#ifndef TERMINAL_DISPLAY_H
#define TERMINAL_DISPLAY_H

#include <string>
#include <iostream>
#include <cstdlib> // Для функции system()

namespace TerminalDisplay {
    //Стили
    const std::string RESET = "\x1b[0m"; //Сброс к стандартному стилю
    const std::string BOLD = "\x1b[1m"; //Жирный стиль
    const std::string FAINT = "\x1b[2m"; //Тусклый стиль
    const std::string ITALIC = "\x1b[3m"; //Курсивный стиль
    const std::string UNDERLINE = "\x1b[4m"; //Подчеркнутый текст

    // Цвета
    const std::string BLACK = "\x1b[30m"; //Черный (Обычный)
    const std::string RED = "\x1b[31m"; //Красный (Ошибки)
    const std::string GREEN = "\x1b[32m"; //Зеленый (Для удовлетворительных значений)
    const std::string YELLOW = "\x1b[33m"; //Желтый (Важная информация)
    const std::string BLUE = "\x1b[34m"; //Синий
    const std::string MAGENTA = "\x1b[35m"; //Пурпурный
    const std::string CYAN = "\x1b[36m"; //Голубой
    const std::string WHITE = "\x1b[37m"; //Белый
    //Яркие цвета
    const std::string BRIGHT_BLACK = "\x1b[90m";  
    const std::string BRIGHT_RED = "\x1b[91m";
    const std::string BRIGHT_GREEN = "\x1b[92m";
    const std::string BRIGHT_YELLOW = "\x1b[93m";
    const std::string BRIGHT_BLUE = "\x1b[94m";
    const std::string BRIGHT_MAGENTA = "\x1b[95m";
    const std::string BRIGHT_CYAN = "\x1b[96m";
    const std::string BRIGHT_WHITE = "\x1b[97m";

    //Цвет фона
    const std::string BG_BLACK = "\x1b[40m";
    const std::string BG_RED = "\x1b[41m";
    const std::string BG_GREEN = "\x1b[42m";
    const std::string BG_YELLOW = "\x1b[43m";
    const std::string BG_BLUE = "\x1b[44m";
    const std::string BG_MAGENTA = "\x1b[45m";
    const std::string BG_CYAN = "\x1b[46m";
    const std::string BG_WHITE = "\x1b[47m";

    // Очистка терминала
    inline void clearScreen() {
#ifdef _WIN32
        system("cls"); // Если ОС Windows
#else
        system("clear"); // В противном случае Linux/Termux/macOS
#endif
    }
}

#endif // TERMINAL_DISPLAY_H
