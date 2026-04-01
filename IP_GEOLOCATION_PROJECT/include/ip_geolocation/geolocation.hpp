#ifndef GEOLOCATION_HPP
#define GEOLOCATION_HPP

#include "ip_geolocation_export.h"
#include <string>
#include <memory>
#include <functional>

namespace ip_geolocation {

// Структура для хранения информации о геолокации
struct IP_GEOLOCATION_API GeoLocation {
    std::string ip;
    std::string country;
    std::string country_code;
    std::string region;
    std::string city;
    std::string postal_code;
    double latitude = 0.0;
    double longitude = 0.0;
    std::string timezone;
    std::string isp;
    std::string organization;
    std::string error_message;
    bool success = false;
    
    std::string toString() const;
};

// Класс для работы с геолокацией
class IP_GEOLOCATION_API GeoLocationService {
public:
    GeoLocationService();
    ~GeoLocationService();
    
    // Настройка API ключа (для онлайн сервисов)
    void setApiKey(const std::string& api_key);
    
    // Настройка пути к локальной базе данных
    void setDatabasePath(const std::string& path);
    
    // Получение геолокации по IP
    GeoLocation getLocation(const std::string& ip);
    
    // Асинхронное получение геолокации
    void getLocationAsync(const std::string& ip, 
                          std::function<void(const GeoLocation&)> callback);
    
    // Проверка доступности сервиса
    bool isAvailable();
    
    // Очистка кэша
    void clearCache();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Вспомогательные функции
IP_GEOLOCATION_API std::string getCurrentIP();
IP_GEOLOCATION_API bool isValidIP(const std::string& ip);

} // namespace ip_geolocation

#endif // GEOLOCATION_HPP
