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
