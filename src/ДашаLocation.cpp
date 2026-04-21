// ДашаLocation.cpp

#include "ДашаLocation.h"
#include <iomanip> //для точного количества знаков после запятой

void GeoLocationData::display() const {
    using namespace TerminalDisplay;
    std::cout << BOLD << GREEN << "\n--- ДАННЫЕ О ГЕОЛОКАЦИИ ---" << RESET << std::endl;
    std::cout << CYAN << "  IP           : " << WHITE << ip << RESET << std::endl; 
    std::cout << CYAN << "  Страна         : " << WHITE << country << " (" << countryCode << ")" << RESET << std::endl;
    std::cout << CYAN << "  Регион       : " << WHITE << regionName << RESET << std::endl;
    std::cout << CYAN << "  Город        : " << WHITE << city << RESET << std::endl;
    std::cout << CYAN << "  Почтовый индекс  : " << WHITE << zip << RESET << std::endl;
    std::cout << CYAN << "  Широта/Долгота      : " << WHITE << std::fixed << std::setprecision(6) << lat << ", " << std::fixed << std::setprecision(6) << lon << RESET << std::endl; //6 знаков после запятой
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
        {"message", p.message} 
        {"mobile", p.mobile},
        {"proxy", p.proxy},
        {"hosting", p.hosting}
    };
}
//Конвертация данных обратно из JSON 
void from_json(const nlohmann::json& j, GeoLocationData& p) {
    j.at("ip").get_to(p.ip);
    j.at("country").get_to(p.country);
    j.at("countryCode").get_to(p.countryCode);
    j.at("regionName").get_to(p.regionName);
    j.at("city").get_to(p.city);
    j.at("zip").get_to(p.zip);
    j.at("lat").get_to(p.lat);
    j.at("lon").get_to(p.lon);
    j.at("timezone").get_to(p.timezone);
    j.at("isp").get_to(p.isp);
    j.at("org").get_to(p.org);
    j.at("as").get_to(p.as);
    j.at("status").get_to(p.status);
    // Так как не всегда будет информация "message"
    if (j.find("message") != j.end() && !j.at("message").is_null()) {
        j.at("message").get_to(p.message);
    }
    else {
        p.message = "NONE"; 
    }
    if (j.find("mobile") != j.end() && !j.at("mobile").is_null()) { p.mobile = j.at("mobile"); }
    if (j.find("proxy") != j.end() && !j.at("proxy").is_null()) { p.proxy = j.at("proxy"); }
    if (j.find("hosting") != j.end() && !j.at("hosting").is_null()) { p.hosting = j.at("hosting"); }
}
