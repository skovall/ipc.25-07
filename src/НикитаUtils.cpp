#include "Utils.h"
#include <iostream> 
bool Utils::VlIPv4(const std::string& ip) {
    const std::regex ipv4_reg(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
    );
    return std::regex_match(ip, ipv4_reg);
}
bool Utils::VlIPv6(const std::string& ip) {
    const std::regex ipv6_reg(
        "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|"  
        "([0-9a-fA-F]{1,4}:){1,7}:|"                   
        "([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|"  
        "([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|" 
        "([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|"
        "([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|" 
        "([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|" 
        "[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|"  
        "::([0-9a-fA-F]{1,4}(:[0-9a-fA-F]{1,4}){1,6})?)" 
    );
    return std::regex_match(ip, ipv6_reg);
}

bool Utils::VlIP(const std::string_view ip) {
    return VlIPv4(ip) || VlIPv6(ip);
}
