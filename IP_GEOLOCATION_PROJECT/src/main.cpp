#include "ip_geolocation/geolocation.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void printUsage(const char* programName) {
    std::cout << "IP Geolocation Tool" << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "  " << programName << " <IP_ADDRESS>" << std::endl;
    std::cout << "  " << programName << " me (для определения вашего IP)" << std::endl;
    std::cout << "  " << programName << " --async <IP_ADDRESS> (асинхронный режим)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "--help" || command == "-h") {
        printUsage(argv[0]);
        return 0;
    }
    
    bool asyncMode = false;
    std::string ip;
    
    if (command == "--async" && argc >= 3) {
        asyncMode = true;
        ip = argv[2];
    } else {
        ip = command;
    }
    
    try {
        ip_geolocation::GeoLocationService service;
        
        // Можно установить API ключ
        // service.setApiKey("your_api_key_here");
        
        // Можно использовать локальную базу данных
        // service.setDatabasePath("./data/GeoLite2-City.mmdb");
        
        if (asyncMode) {
            std::cout << "Запрашиваем геолокацию для IP: " << ip << " (асинхронно)..." << std::endl;
            
            service.getLocationAsync(ip, [](const ip_geolocation::GeoLocation& location) {
                std::cout << "Результат:" << std::endl;
                std::cout << location.toString() << std::endl;
            });
            
            // Имитация другой работы
            std::cout << "Ожидание результатов..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            std::cout << "Запрашиваем геолокацию для IP: " << ip << "..." << std::endl;
            
            auto location = service.getLocation(ip);
            std::cout << location.toString() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
