// src/ConfigManager.h
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <iostream>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;


class ConfigManager {
public:
    // ФУНКЦИЯ: Вызывается при создании объекта. Принимает путь к файлу настроек.
    // Автоматически загружает настройки из файла, либо создаёт файл с дефолтами.
    ConfigManager(const std::string& configFilePath);

    // ФУНКЦИЯ: Загружает настройки из файла.
    // Если файл не найден или повреждён — создаёт файл с настройками по умолчанию.
    bool loadConfig();

    // ФУНКЦИЯ: Сохраняет текущие настройки в файл.
    bool saveConfig() const;

    // ФУНКЦИЯ: Сбрасывает настройки до значений по умолчанию и сохраняет файл.
    void resetToDefaults();

    // ФУНКЦИЯ: Получает значение по ключу с поддержкой разных типов.
    // Если ключ не найден или тип не совпадает — возвращает defaultValue.
    template <typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        if (configData.count(key)) {
            try {
                return configData.at(key).get<T>();
            }
            catch (const json::type_error& e) {
                std::cerr << "Ошибка типа для ключа '" << key << "': " << e.what() << std::endl;
                return defaultValue;
            }
        }
        return defaultValue;
    }

    // ФУНКЦИЯ: Записывает новое значение для указанного ключа.
    template <typename T>
    void setValue(const std::string& key, const T& value) {
        configData[key] = value;
    }

    // ФУНКЦИЯ: Выводит текущие настройки на экран.
    void displayConfig(std::ostream& os = std::cout) const;

private:
    std::string filePath;  // Путь к файлу настроек
    json configData;       // JSON-объект с текущими настройками в памяти

    // ФУНКЦИЯ: Вызывается при первом запуске или при повреждении файла.
    void setDefaults();
};

#endif
