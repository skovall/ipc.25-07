#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>
#include "Location.h"

class History { // Класс для управления историей поиска
public:
    explicit History(const std::string& file_name);  // Конструктор, explicit запрещает неявное преобразование

    bool load();    
    bool save() const;

    void addEntry(const GeoLocationData& data);  // Добавить запись (константная ссылка - без копирования)

    void clear();

    const std::vector<GeoLocationData>& entries() const;  // Получить все записи (только для чтения)

    void display() const; 

    void setMaxEntries(size_t max_entries);  // Установить максимальный размер истории

private:
    std::string file_name_;
    std::vector<GeoLocationData> entries_;
    size_t max_entries_;
};

#endif
