#ifndef HISTORY_H
#define HISTORY_H

#include <vector>
#include <string>
#include "Location.h"

class History {
public:
    explicit History(const std::string& file_name);

    bool load();
    bool save() const;

    void addEntry(const GeoLocationData& data);

    void clear();

    const std::vector<GeoLocationData>& entries() const;

    void display() const;

    void setMaxEntries(size_t max_entries);

private:
    std::string file_name_;
    std::vector<GeoLocationData> entries_;
    size_t max_entries_;
};

#endif
