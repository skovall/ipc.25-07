#include "GeoLocator.h"
#include "TerminalDisplay.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

GeoLocator::GeoLocator(HttpRequest& http_client)
    : m_httpClient(http_client)
    , m_lastSuccess(false)
{
}

std::string GeoLocator::buildApiUrl(const std::string& ip_address) const {
    std::string url = "http://ip-api.com/json/" + ip_address;
    url += "?fields=status,message,country,countryCode,regionName,city,";
    url += "zip,lat,lon,timezone,isp,org,as,mobile,proxy,hosting,query";
    return url;
}

GeoLocationData GeoLocator::getDataForIp(const std::string& ip_address) {
    GeoLocationData result;
    result.ip = ip_address;
    
    std::string api_url = buildApiUrl(ip_address);
    
    try {
        std::string json_response = m_httpClient.get(api_url);
        
        if (json_response.empty()) {
            m_lastError = "Пустой ответ от API";
            m_lastSuccess = false;
            result.status = "fail";
            result.message = m_lastError;
            return result;
        }
        
        result = parseApiResponse(json_response, ip_address);
        
        if (result.status == "success") {
            m_lastSuccess = true;
            m_lastError.clear();
        } else {
            m_lastSuccess = false;
            m_lastError = result.message;
        }
        
    } catch (const std::exception& e) {
        m_lastSuccess = false;
        m_lastError = "HTTP ошибка: " + std::string(e.what());
        result.status = "fail";
        result.message = m_lastError;
    }
    
    return result;
}

GeoLocationData GeoLocator::parseApiResponse(const std::string& json_string, 
                                             const std::string& requested_ip) {
    GeoLocationData data;
    data.ip = requested_ip;
    
    try {
        json api_data = json::parse(json_string);
        
        std::string status = api_data.value("status", "fail");
        data.status = status;
        
        if (status == "success") {
            data.country      = api_data.value("country", "Неизвестно");
            data.countryCode  = api_data.value("countryCode", "N/A");
            data.regionName   = api_data.value("regionName", "Неизвестно");
            data.city         = api_data.value("city", "Неизвестно");
            data.zip          = api_data.value("zip", "N/A");
            data.lat          = api_data.value("lat", 0.0);
            data.lon          = api_data.value("lon", 0.0);
            data.timezone     = api_data.value("timezone", "N/A");
            data.isp          = api_data.value("isp", "Неизвестно");
            data.org          = api_data.value("org", "Неизвестно");
            data.as           = api_data.value("as", "N/A");
            data.mobile       = api_data.value("mobile", false);
            data.proxy        = api_data.value("proxy", false);
            data.hosting      = api_data.value("hosting", false);
            data.message      = "";
            
            if (api_data.contains("query")) {
                std::string detected_ip = api_data["query"];
                if (detected_ip != requested_ip) {
                    data.ip = detected_ip;
                }
            }
        } else {
            data.message = api_data.value("message", "Неизвестная ошибка");
            data.country = "Ошибка";
            data.city = "Ошибка";
        }
        
    } catch (const json::parse_error& e) {
        data.status = "fail";
        data.message = "Ошибка парсинга JSON: " + std::string(e.what());
        data.country = "Ошибка";
    }
    
    return data;
}

std::string GeoLocator::getMyPublicIp() {
    try {
        std::string url = "https://api.ipify.org";
        std::string my_ip = m_httpClient.get(url);
        
        while (!my_ip.empty() && (my_ip.back() == '\n' || my_ip.back() == '\r')) {
            my_ip.pop_back();
        }
        
        if (!my_ip.empty()) {
            m_lastSuccess = true;
            m_lastError.clear();
            return my_ip;
        } else {
            m_lastSuccess = false;
            m_lastError = "Не удалось определить IP (пустой ответ)";
            return "";
        }
        
    } catch (const std::exception& e) {
        m_lastSuccess = false;
        m_lastError = "Ошибка при получении IP: " + std::string(e.what());
        return "";
    }
}

bool GeoLocator::isLastRequestSuccessful() const {
    return m_lastSuccess;
}

std::string GeoLocator::getLastErrorMessage() const {
    return m_lastError;
}
