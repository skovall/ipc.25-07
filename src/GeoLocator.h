#ifndef GEO_LOCATOR_H
#define GEO_LOCATOR_H

#include <string>
#include "Location.h"
#include "HttpRequest.h"

class GeoLocator {
public:
    explicit GeoLocator(HttpRequest& http_client);
    
    GeoLocationData getDataForIp(const std::string& ip_address);
    std::string getMyPublicIp();
    bool isLastRequestSuccessful() const;
    std::string getLastErrorMessage() const;

private:
    HttpRequest& m_httpClient;
    std::string m_lastError;
    bool m_lastSuccess;
    
    GeoLocationData parseApiResponse(const std::string& json_string, 
                                     const std::string& requested_ip);
    std::string buildApiUrl(const std::string& ip_address) const;
};

#endif
