#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <regex>
class Utils{
public:
  static bool VlIPv4(const std::string& ip);
  static bool VlIPv6(const std::string& ip);
  static bool VlIP(const std::string& ip);
};

#endif 
