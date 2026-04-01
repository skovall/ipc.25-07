#include "ip_geolocation/geolocation.hpp"
#include "platform/platform_utils.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <regex>
#include <fstream>
#include <sstream>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

using json = nlohmann::json;

namespace ip_geolocation {

// Вспомогательная функция для записи данных из CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string GeoLocation::toString() const {
    std::stringstream ss;
    ss << "IP: " << ip << std::endl;
    ss << "Country: " << country << " (" << country_code << ")" << std::endl;
    ss << "Region: " << region << std::endl;
    ss << "City: " << city << std::endl;
    ss << "Postal Code: " << postal_code << std::endl;
    ss << "Coordinates: " << latitude << ", " << longitude << std::endl;
    ss << "Timezone: " << timezone << std::endl;
    ss << "ISP: " << isp << std::endl;
    ss << "Organization: " << organization << std::endl;
    ss << "Success: " << (success ? "Yes" : "No");
    if (!error_message.empty()) {
        ss << std::endl << "Error: " << error_message;
    }
    return ss.str();
}

class GeoLocationService::Impl {
public:
    Impl() : curl_handle(nullptr) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_handle = curl_easy_init();
    }
    
    ~Impl() {
        if (curl_handle) {
            curl_easy_cleanup(curl_handle);
        }
        curl_global_cleanup();
    }
    
    GeoLocation getLocation(const std::string& ip) {
        // Проверка кэша
        {
            std::lock_guard<std::mutex> lock(cache_mutex);
            auto it = cache.find(ip);
            if (it != cache.end()) {
                auto age = std::chrono::steady_clock::now() - it->second.timestamp;
                if (age < cache_duration) {
                    return it->second.location;
                }
            }
        }
        
        GeoLocation result;
        result.ip = ip;
        
        if (!isValidIP(ip) && ip != "me") {
            result.error_message = "Invalid IP address";
            return result;
        }
        
        // Использование локальной базы данных
        if (!database_path.empty()) {
            result = queryLocalDatabase(ip);
        }
        
        // Если локальная база не дала результата или не настроена, используем онлайн сервисы
        if (!result.success) {
            result = queryOnlineServices(ip);
        }
        
        // Кэширование результата
        if (result.success) {
            std::lock_guard<std::mutex> lock(cache_mutex);
            CacheEntry entry;
            entry.location = result;
            entry.timestamp = std::chrono::steady_clock::now();
            cache[ip] = entry;
        }
        
        return result;
    }
    
    void setApiKey(const std::string& key) {
        api_key = key;
    }
    
    void setDatabasePath(const std::string& path) {
        database_path = path;
    }
    
    bool isAvailable() {
        if (!database_path.empty() && !queryLocalDatabase("8.8.8.8").error_message.empty()) {
            return true;
        }
        
        GeoLocation test = queryOnlineServices("8.8.8.8");
        return test.success;
    }
    
    void clearCache() {
        std::lock_guard<std::mutex> lock(cache_mutex);
        cache.clear();
    }

private:
    CURL* curl_handle;
    std::string api_key;
    std::string database_path;
    
    struct CacheEntry {
        GeoLocation location;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::unordered_map<std::string, CacheEntry> cache;
    std::mutex cache_mutex;
    std::chrono::hours cache_duration{24}; // Кэш на 24 часа
    
    GeoLocation queryLocalDatabase(const std::string& ip) {
        GeoLocation result;
        result.ip = ip;
        
        // Здесь должна быть реализация работы с локальной базой данных
        // Например, с использованием libmaxminddb для GeoIP2
        // Для простоты возвращаем пустой результат
        
        return result;
    }
    
    GeoLocation queryOnlineServices(const std::string& ip) {
        GeoLocation result;
        result.ip = ip;
        
        // Пробуем разные сервисы
        std::vector<std::string> services;
        
        // ip-api.com (бесплатно, без ключа, ограничение 45 запросов в минуту)
        services.push_back("http://ip-api.com/json/" + ip + "?fields=status,message,country,countryCode,region,regionName,city,zip,lat,lon,timezone,isp,org,query");
        
        // ipinfo.io (требуется ключ)
        if (!api_key.empty()) {
            services.push_back("https://ipinfo.io/" + ip + "/json?token=" + api_key);
        }
        
        // freegeoip.app (ограничение 15000 запросов в час)
        services.push_back("https://freegeoip.app/json/" + ip);
        
        for (const auto& service_url : services) {
            result = queryService(service_url);
            if (result.success) {
                break;
            }
        }
        
        return result;
    }
    
    GeoLocation queryService(const std::string& url) {
        GeoLocation result;
        std::string response;
        
        if (!curl_handle) {
            result.error_message = "CURL initialization failed";
            return result;
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "IPGeolocationLibrary/1.0");
        
        // Добавляем заголовки для некоторых сервисов
        struct curl_slist* headers = nullptr;
        if (url.find("freegeoip.app") != std::string::npos) {
            headers = curl_slist_append(headers, "Accept: application/json");
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
        }
        
        CURLcode res = curl_easy_perform(curl_handle);
        
        if (headers) {
            curl_slist_free_all(headers);
        }
        
        if (res != CURLE_OK) {
            result.error_message = curl_easy_strerror(res);
            return result;
        }
        
        // Парсим JSON ответ
        try {
            auto json_response = json::parse(response);
            
            if (url.find("ip-api.com") != std::string::npos) {
                // Парсинг ответа от ip-api.com
                if (json_response.contains("status") && json_response["status"] == "success") {
                    result.success = true;
                    result.country = json_response.value("country", "");
                    result.country_code = json_response.value("countryCode", "");
                    result.region = json_response.value("regionName", "");
                    result.city = json_response.value("city", "");
                    result.postal_code = json_response.value("zip", "");
                    result.latitude = json_response.value("lat", 0.0);
                    result.longitude = json_response.value("lon", 0.0);
                    result.timezone = json_response.value("timezone", "");
                    result.isp = json_response.value("isp", "");
                    result.organization = json_response.value("org", "");
                    result.ip = json_response.value("query", result.ip);
                } else {
                    result.error_message = json_response.value("message", "Unknown error");
                }
            }
            else if (url.find("ipinfo.io") != std::string::npos) {
                // Парсинг ответа от ipinfo.io
                if (!json_response.contains("error")) {
                    result.success = true;
                    result.country = json_response.value("country", "");
                    result.region = json_response.value("region", "");
                    result.city = json_response.value("city", "");
                    result.postal_code = json_response.value("postal", "");
                    
                    if (json_response.contains("loc")) {
                        auto loc = json_response["loc"].get<std::string>();
                        auto comma_pos = loc.find(',');
                        if (comma_pos != std::string::npos) {
                            result.latitude = std::stod(loc.substr(0, comma_pos));
                            result.longitude = std::stod(loc.substr(comma_pos + 1));
                        }
                    }
                    
                    result.timezone = json_response.value("timezone", "");
                    result.isp = json_response.value("org", "");
                    result.organization = json_response.value("org", "");
                    result.ip = json_response.value("ip", result.ip);
                } else {
                    result.error_message = "API error";
                }
            }
            else if (url.find("freegeoip.app") != std::string::npos) {
                // Парсинг ответа от freegeoip.app
                if (!json_response.contains("error")) {
                    result.success = true;
                    result.country = json_response.value("country_name", "");
                    result.country_code = json_response.value("country_code", "");
                    result.region = json_response.value("region_name", "");
                    result.city = json_response.value("city", "");
                    result.postal_code = json_response.value("zip_code", "");
                    result.latitude = json_response.value("latitude", 0.0);
                    result.longitude = json_response.value("longitude", 0.0);
                    result.timezone = json_response.value("timezone", "");
                    result.ip = json_response.value("ip", result.ip);
                } else {
                    result.error_message = "API error";
                }
            }
        }
        catch (const std::exception& e) {
            result.error_message = "JSON parse error: " + std::string(e.what());
        }
        
        return result;
    }
};

// Реализация GeoLocationService
GeoLocationService::GeoLocationService() : pImpl(std::make_unique<Impl>()) {}
GeoLocationService::~GeoLocationService() = default;

void GeoLocationService::setApiKey(const std::string& api_key) {
    pImpl->setApiKey(api_key);
}

void GeoLocationService::setDatabasePath(const std::string& path) {
    pImpl->setDatabasePath(path);
}

GeoLocation GeoLocationService::getLocation(const std::string& ip) {
    return pImpl->getLocation(ip);
}

void GeoLocationService::getLocationAsync(const std::string& ip, 
                                          std::function<void(const GeoLocation&)> callback) {
    std::thread([this, ip, callback]() {
        auto result = this->getLocation(ip);
        callback(result);
    }).detach();
}

bool GeoLocationService::isAvailable() {
    return pImpl->isAvailable();
}

void GeoLocationService::clearCache() {
    pImpl->clearCache();
}

// Вспомогательные функции
std::string getCurrentIP() {
    GeoLocationService service;
    auto location = service.getLocation("me");
    return location.ip;
}

bool isValidIP(const std::string& ip) {
    if (ip == "me") return true;
    
    std::regex ipv4_pattern(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    std::regex ipv6_pattern(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$)");
    
    if (!std::regex_match(ip, ipv4_pattern) && !std::regex_match(ip, ipv6_pattern)) {
        return false;
    }
    
    // Дополнительная проверка для IPv4
    if (std::regex_match(ip, ipv4_pattern)) {
        std::stringstream ss(ip);
        std::string segment;
        while (std::getline(ss, segment, '.')) {
            int value = std::stoi(segment);
            if (value < 0 || value > 255) {
                return false;
            }
        }
    }
    
    return true;
}

} // namespace ip_geolocation
