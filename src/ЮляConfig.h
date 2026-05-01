// src/ConfigManager.h
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <iostream> 
#include <string>   
#include <fstream>  // Для чтения и записи файлов
#include "nlohmann/json.hpp" 

using json = nlohmann::json;



class ConfigManager {
public:
        // ФУНКЦИЯ: Вызывается при создании объекта. Принимает путь к файлу настроек.
    ConfigManager(const std::string& configFilePath);

        // ФУНКЦИЯ: Загружает настройки из файла.
    bool loadConfig();

        // ФУНКЦИЯ: Сохраняет текущие настройки в файл. 
    bool saveConfig() const;

        // ФУНКЦИЯ: Сброс настроек.
    void resetToDefaults();



        // ФУНКЦИЯ: Получает значение по ключу. (принимает разные типы значений)
    template <typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
            
        //Проверка: существует ли такой ключ
        if (configData.count(key)) {
            try {
                return configData.at(key).get<T>();
            }

            // Если тип не совпал, ловим ошибку
            catch (const json::type_error& e) {
                std::cerr << "Ошибка типа для ключа '" << key << "' в конфигурации: " << e.what() << std::endl;
                return defaultValue;
            }
        }
        return defaultValue; // Если ключа нет, тоже возвращает значение по умолчанию
    }

 
        // ФУНКЦИЯ: Записывает новое значение для настройки
    template <typename T>
    void setValue(const std::string& key, const T& value) {
        configData[key] = value;
    }

        // ФУНКЦИЯ: Выводит текущие настройки на экран (полезно для поиска ошибок)
    void displayConfig(std::ostream& os = std::cout) const;

private:

    std::string filePath;  // Путь к файлу настроек
    json configData;       // Объект JSON, где в памяти хранятся все наши настройки

    void setDefaults();   // ФУНКЦИЯ: Настройки по умолчанию
};

#endif 


