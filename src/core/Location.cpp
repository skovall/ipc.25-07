#include "Location.h"
#include <iomanip> //для точного количества знаков после запятой


#ifdef _WIN32
#include <windows.h>
#endif

void GeoLocationData::display() const {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    // Включаем поддержку ANSI escape-последовательностей
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
#endif

    using namespace TerminalDisplay;
    std::cout << BOLD << GREEN << "\n--- ДАННЫЕ О ГЕОЛОКАЦИИ ---" << RESET << std::endl;
    std::cout << CYAN << "  IP           : " << WHITE << ip << RESET << std::endl;   
    std::cout << CYAN << "  Страна       : " << WHITE << country << " (" << countryCode << ")" << RESET << std::endl;  
    std::cout << CYAN << "  Регион       : " << WHITE << regionName << RESET << std::endl;  
    std::cout << CYAN << "  Город        : " << WHITE << city << RESET << std::endl;  
    std::cout << CYAN << "  Почтовый индекс : " << WHITE << zip << RESET << std::endl;  
    std::cout << CYAN << "  Широта/Долгота  : " << WHITE << std::fixed << std::setprecision(6) << lat << ", " << std::setprecision(6) << lon << RESET << std::endl;
    std::cout << CYAN << "  Часовой пояс : " << WHITE << timezone << RESET << std::endl;  
    std::cout << CYAN << "  Провайдер    : " << WHITE << isp << RESET << std::endl;  
    std::cout << CYAN << "  Организация  : " << WHITE << org << RESET << std::endl;  
    std::cout << CYAN << "  AS           : " << WHITE << as << RESET << std::endl;      
    std::cout << CYAN << "  Мобильное    : " << WHITE << (mobile ? "Да" : "Нет") << RESET << std::endl;  
    std::cout << CYAN << "  Прокси/VPN   : " << WHITE << (proxy ? "Да" : "Нет") << RESET << std::endl;  
    std::cout << CYAN << "  Хостинг      : " << WHITE << (hosting ? "Да" : "Нет") << RESET << std::endl;  
    std::cout << CYAN << "  Статус       : " << (status == "success" ? TerminalDisplay::BRIGHT_GREEN : TerminalDisplay::BRIGHT_RED) << status << RESET << std::endl;
    std::cout << BOLD << GREEN << "------------------------------------" << RESET << std::endl;
}

//Конвертация данных в JSON
void to_json(nlohmann::json& j, const GeoLocationData& p) {
    j = nlohmann::json{
        {"ip", p.ip},
        {"country", p.country},
        {"countryCode", p.countryCode},
        {"regionName", p.regionName},
        {"city", p.city},
        {"zip", p.zip},
        {"lat", p.lat},
        {"lon", p.lon},
        {"timezone", p.timezone},
        {"isp", p.isp},
        {"org", p.org},
        {"as", p.as},
        {"status", p.status},
        {"message", p.message},   
        {"mobile", p.mobile},
        {"proxy", p.proxy},
        {"hosting", p.hosting}
    };
}

//Конвертация данных обратно из JSON
void from_json(const nlohmann::json& j, GeoLocationData& p) {
    p.ip = j.value("ip", "");
    p.country = j.value("country", "");
    p.countryCode = j.value("countryCode", "");
    p.regionName = j.value("regionName", "");
    p.city = j.value("city", "");
    p.zip = j.value("zip", "");
    p.lat = j.value("lat", 0.0);
    p.lon = j.value("lon", 0.0);
    p.timezone = j.value("timezone", "");
    p.isp = j.value("isp", "");
    p.org = j.value("org", "");
    p.as = j.value("as", "");
    p.status = j.value("status", "error");
    p.message = j.value("message", "NONE");
    p.mobile = j.value("mobile", false);
    p.proxy = j.value("proxy", false);
    p.hosting = j.value("hosting", false);
}
