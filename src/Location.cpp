// Location.cpp

#include "Location.h"
#include <iomanip> //для точного количества знаков после запятой

void GeoLocationData::display() const {
    using namespace TerminalDisplay;
    std::cout << BOLD << GREEN << "\n--- ДАННЫЕ О ГЕОЛОКАЦИИ ---" << RESET << std::endl;
    std::cout << CYAN << "  IP           : " << WHITE << ip << RESET << std::endl; 
    std::cout << CYAN << "  Страна         : " << WHITE << country << " (" << countryCode << ")" << RESET << std::endl;
    std::cout << CYAN << "  Регион       : " << WHITE << regionName << RESET << std::endl;
    std::cout << CYAN << "  Город        : " << WHITE << city << RESET << std::endl;
    std::cout << CYAN << "  Почтовый индекс  : " << WHITE << zip << RESET << std::endl;
    std::cout << CYAN << "  Широта/Долгота      : " << WHITE << std::fixed << std::setprecision(6) << lat << ", " << std::setprecision(6) << lon << RESET << std::endl; //6 знаков после запятой
    std::cout << CYAN << "  Часовой пояс : " << WHITE << timezone << RESET << std::endl;
    std::cout << CYAN << "  Провайдер (ISP - Internet Service Provider)          : " << WHITE << isp << RESET << std::endl;
    std::cout << CYAN << "  Организация : " << WHITE << org << RESET << std::endl;
    std::cout << CYAN << "  Автономная система (AS)           : " << WHITE << as << RESET << std::endl;    
    std::cout << CYAN << "  Мобильное соединение       : " << WHITE << (mobile ? "Да" : "Нет") << RESET << std::endl;
    std::cout << CYAN << "  Прокси-сервер/VPN        : " << WHITE << (proxy ? "Да" : "Нет") << RESET << std::endl;
    std::cout << CYAN << "  Hosting (дата-центр)      : " << WHITE << (hosting ? "Да" : "Нет") << RESET << std::endl;

    std::cout << CYAN << "  Статус       : " << (status == "success" ? TerminalDisplay::BRIGHT_GREEN : TerminalDisplay::BRIGHT_RED) << status << RESET << std::endl; //при успешном выполнении - зеленый цвет, противоположный - красный
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
