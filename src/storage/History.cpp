#include "History.h"
#include "TerminalDisplay.h"
#include <fstream>
#include <iomanip>

using json = nlohmann::json;

History::History(const std::string& file_name)
    : file_name_(file_name)
    , max_entries_(100)
{
}

bool History::load() {
    std::ifstream file(file_name_);

    if (!file.is_open()) {
        return false;
    }

    try {
        json j;
        file >> j;

        entries_.clear();

        if (j.is_array()) {
            for (const auto& item : j) {
                entries_.push_back(item.get<GeoLocationData>());
            }
        }

        return true;
    }
    catch (...) {
        return false;
    }
}

bool History::save() const {
    std::ofstream file(file_name_);

    if (!file.is_open()) {
        return false;
    }

    try {
        json j = entries_;
        file << std::setw(4) << j;
        return true;
    }
    catch (...) {
        return false;
    }
}

void History::addEntry(const GeoLocationData& data) {
    entries_.push_back(data);

    if (entries_.size() > max_entries_) {
        entries_.erase(entries_.begin());
    }
}

void History::clear() {
    entries_.clear();
}

const std::vector<GeoLocationData>& History::entries() const {
    return entries_;
}
