
#include "ConfigManager.h"
#include <iostream>
#include <iomanip>
#include "TerminalDisplay.h"


// ФУНКЦИЯ: Конструктор класса ConfigManager
ConfigManager::ConfigManager(const std::string& configFilePath)
    : filePath(configFilePath) {
    setDefaults();  
    loadConfig();   // пытаемся загрузить реальный файл (перезапишет дефолты если файл есть)
}


// ФУНКЦИЯ: Настройки по умолчанию
void ConfigManager::setDefaults() {
    // Настраиваем API по умолчанию
    configData["api_default"] = "ip-api.com";
    configData["api_keys"]["ip-api.com"] = "";
    configData["api_keys"]["ipinfo.io"] = "YOUR_IPINFO_API_KEY";
    configData["api_keys"]["abstractapi.com"] = "YOUR_ABSTRACTAPI_API_KEY";

    // Общие настройки программы
    configData["display_verbose"] = false;              // Детальный вывод отключен
    configData["history_file"] = "ip_nose_history.json"; // Файл истории
    configData["max_history_entries"] = 100;            // Лимит истории
    configData["output_format"] = "text";               // Формат вывода текста
    configData["always_check_vpn"] = true;            // Доп параметр для впн
    
}


// ФУНКЦИЯ: Загрузка настроек из файла
bool ConfigManager::loadConfig() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        saveConfig(); // <-- автосохранение при первом запуске
        return false;
    }

    // Проверяем что файл не пустой
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        setDefaults();
        saveConfig(); // Перезаписываем пустой файл дефолтами
        return false;
    }
    file.seekg(0, std::ios::beg);
    file.clear();

    try {
        file >> configData; // Считываем весь JSON из файла в configData
        std::cout << TerminalDisplay::BRIGHT_GREEN
                  << "Настройки загружены из '" << filePath << "'."
                  << TerminalDisplay::RESET << std::endl;
        return true;
    }
    catch (const json::parse_error& e) {
        // Сломанный JSON — откатываемся на дефолты и пересохраняем
        std::cerr << TerminalDisplay::RED
                  << "Ошибка чтения JSON: " << e.what()
                  << ". Используем значения по умолчанию."
                  << TerminalDisplay::RESET << std::endl;
        setDefaults();
        saveConfig(); // Восстанавливаем файл
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << TerminalDisplay::RED
                  << "Непредвиденная ошибка: " << e.what()
                  << TerminalDisplay::RESET << std::endl;
        setDefaults();
        return false;
    }
}


// ФУНКЦИЯ: Сохранение настроек в файл
bool ConfigManager::saveConfig() const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << TerminalDisplay::RED
                  << "Ошибка: Не удалось открыть файл '" << filePath << "' для сохранения."
                  << TerminalDisplay::RESET << std::endl;
        return false;
    }

    try {
        // Записываем JSON в файл с отступами (4 пробела)
        file << std::setw(4) << configData << std::endl;
        std::cout << TerminalDisplay::BRIGHT_GREEN
                  << "Настройки сохранены в '" << filePath << "'."
                  << TerminalDisplay::RESET << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << TerminalDisplay::RED
                  << "Ошибка при сохранении: " << e.what()
                  << TerminalDisplay::RESET << std::endl;
        return false;
    }
}


// ФУНКЦИЯ: Сброс настроек
void ConfigManager::resetToDefaults() {
    setDefaults();
    saveConfig(); // Сразу сохраняем сброшенные настройки в файл
    std::cout << TerminalDisplay::BRIGHT_YELLOW
              << "Настройки сброшены по умолчанию."
              << TerminalDisplay::RESET << std::endl;
}


// ФУНКЦИЯ: Показывает текущие настройки пользователю
void ConfigManager::displayConfig(std::ostream& os) const {
    using namespace TerminalDisplay;
    os << BOLD << BRIGHT_CYAN << "\n--- Текущие настройки ---" << RESET << std::endl;
    os << BRIGHT_YELLOW << std::setw(4) << configData << RESET << std::endl;
    os << BOLD << BRIGHT_CYAN << "------------------------------" << RESET << std::endl;
}
