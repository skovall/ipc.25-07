#include "ConfigManager.h"
#include <iostream>

ConfigManager::ConfigManager(const std::string& configFilePath)
    : filePath(configFilePath)
{
    loadConfig();
}

bool ConfigManager::loadConfig() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        setDefaults();
        saveConfig();
        return false;
    }
    
    try {
        file >> configData;
        return true;
    } catch (const json::parse_error& e) {
        std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
        setDefaults();
        return false;
    }
}

bool ConfigManager::saveConfig() const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << filePath << std::endl;
        return false;
    }
    
    file << configData.dump(4);
    return true;
}

void ConfigManager::resetToDefaults() {
    setDefaults();
    saveConfig();
}

void ConfigManager::setDefaults() {
    configData = json{
        {"api_url", "http://ip-api.com/json/"},
        {"timeout_seconds", 10},
        {"max_retries", 3},
        {"enable_cache", true}
    };
}

void ConfigManager::displayConfig(std::ostream& os) const {
    os << "=== Текущая конфигурация ===" << std::endl;
    os << "Файл: " << filePath << std::endl;
    os << configData.dump(4) << std::endl;
}
