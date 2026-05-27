#include <iostream>
#include <string>

#include "ConfigManager.h"
#include "GeoLocator.h"
#include "History.h"
#include "HttpRequest.h"
#include "Utils.h"
#include "TerminalDisplay.h"

int main() {
#ifdef _WIN32
    // Устанавливаем UTF-8 для ввода/вывода
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Включаем виртуальные терминалы для цветов
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    using namespace TerminalDisplay;

    try {

        ConfigManager config("config.json");
        config.loadConfig();
        HttpRequest http;
        bool verbose =
            config.getValue<bool>("display_verbose", false);
        http.setVerbose(verbose);
        GeoLocator locator(http);
        std::string history_file =
            config.getValue<std::string>(
                "history_file",
                "history.json"
            );

        History history(history_file);

        history.load();

        std::cout << BOLD << BRIGHT_GREEN;
        std::cout << "IP GEOLOCATION TOOL\n";
        std::cout << RESET;

        while (true) {
            std::cout << "\n";
            std::cout << "1. Lookup IP\n";
            std::cout << "2. My public IP\n";
            std::cout << "3. Show history\n";
            std::cout << "4. Show config\n";
            std::cout << "5. Exit\n";
            std::cout << "> ";
            int choice = 0;
            std::cin >> choice;

            if (!std::cin) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            if (choice == 1) {
                std::string ip;
                std::cout << "Enter IP: ";
                std::cin >> ip;
                    
                if (!Utils::VlIP(ip)) {
                    std::cout << BRIGHT_RED;
                    std::cout << "Invalid IP\n";
                    std::cout << RESET;
                    continue;
                }

                GeoLocationData data = locator.getDataForIp(ip);
                data.display();
                history.addEntry(data);
                history.save();
            }

            else if (choice == 2) {
                std::string my_ip = locator.getMyPublicIp();

                if (my_ip.empty()) {
                    std::cout << BRIGHT_RED;
                    std::cout
                        << locator.getLastErrorMessage()
                        << "\n";
                    std::cout << RESET;
                    continue;
                }

                std::cout << "Your IP: ";
                std::cout << my_ip << "\n";
                GeoLocationData data =
                    locator.getDataForIp(my_ip);
                data.display();
                history.addEntry(data);
                history.save();
            }

            else if (choice == 3) {
                history.display();
            }

            else if (choice == 4) {
                config.displayConfig();
            }

            else if (choice == 5) {
                break;
            }
        }
    }

    catch (const std::exception& e) {
        std::cerr << "Fatal error: ";
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
