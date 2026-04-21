#include "Utils.h"
#include <regex>
#include <string_view>

namespace {
    #if __cplusplus >= 202002L
    constexpr std::regex ipv4_reg(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
    );
    
    constexpr std::regex ipv6_reg(
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
    #else
    const std::regex ipv4_reg(
        "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
    );
    
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
    #endif
}

bool Utils::VlIPv4(const std::string_view ip) noexcept {
    return std::regex_match(ip.begin(), ip.end(), ipv4_reg);
}

bool Utils::VlIPv6(const std::string_view ip) noexcept {
    return std::regex_match(ip.begin(), ip.end(), ipv6_reg);
}

bool Utils::VlIP(const std::string_view ip) noexcept {
    return VlIPv4(ip) || VlIPv6(ip);
}
