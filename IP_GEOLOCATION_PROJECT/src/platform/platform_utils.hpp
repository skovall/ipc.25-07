#ifndef PLATFORM_UTILS_HPP
#define PLATFORM_UTILS_HPP

#include <string>

namespace ip_geolocation {
namespace platform {

// Получение пути к временной директории
std::string getTempDirectory();

// Получение пути к директории приложения
std::string getAppDataDirectory();

// Создание директории (кроссплатформенно)
bool createDirectory(const std::string& path);

// Проверка существования файла
bool fileExists(const std::string& path);

// Получение размера файла
size_t getFileSize(const std::string& path);

// Получение имени хоста
std::string getHostName();

} // namespace platform
} // namespace ip_geolocation

#endif // PLATFORM_UTILS_HPP
