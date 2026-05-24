#ifndef UTILS_H
#define UTILS_H

#include <string_view>

class Utils {
public:
    static bool VlIPv4(const std::string_view ip) noexcept;
    static bool VlIPv6(const std::string_view ip) noexcept;
    static bool VlIP(const std::string_view ip) noexcept;
};

#endif // UTILS_H
