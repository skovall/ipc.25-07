#ifndef IP_GEOLOCATION_EXPORT_H
#define IP_GEOLOCATION_EXPORT_H

#ifdef _WIN32
    #ifdef IP_GEOLOCATION_EXPORTS
        #define IP_GEOLOCATION_API __declspec(dllexport)
    #else
        #define IP_GEOLOCATION_API __declspec(dllimport)
    #endif
#else
    #define IP_GEOLOCATION_API __attribute__((visibility("default")))
#endif

#endif // IP_GEOLOCATION_EXPORT_H
