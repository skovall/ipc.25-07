#include "platform_utils.hpp"
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <shlobj.h>
    #pragma comment(lib, "shell32.lib")
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#endif

namespace ip_geolocation {
namespace platform {

std::string getTempDirectory() {
#ifdef _WIN32
    char temp_path[MAX_PATH];
    if (GetTempPathA(MAX_PATH, temp_path)) {
        return std::string(temp_path);
    }
    return "C:\\Windows\\Temp\\";
#else
    const char* tmpdir = getenv("TMPDIR");
    if (tmpdir) return tmpdir;
    return "/tmp/";
#endif
}

std::string getAppDataDirectory() {
#ifdef _WIN32
    char app_data_path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, app_data_path) == S_OK) {
        return std::string(app_data_path) + "\\ip_geolocation\\";
    }
    return getTempDirectory() + "ip_geolocation\\";
#else
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return std::string(home) + "/.ip_geolocation/";
#endif
}

bool createDirectory(const std::string& path) {
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
#else
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

size_t getFileSize(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return buffer.st_size;
    }
    return 0;
}

std::string getHostName() {
    char hostname[256];
#ifdef _WIN32
    DWORD size = sizeof(hostname);
    if (GetComputerNameA(hostname, &size)) {
        return std::string(hostname, size);
    }
#else
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
#endif
    return "unknown";
}

} // namespace platform
} // namespace ip_geolocation
