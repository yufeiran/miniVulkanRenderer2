#include"assertPath.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace mini
{

    std::filesystem::path getProjectRoot() {
#ifdef _WIN32 
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return std::filesystem::path(buffer).parent_path();
#else
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count != -1) {
            buffer[count] = '\0';
            return std::filesystem::path(buffer).parent_path();
        }
        return std::filesystem::current_path();
#endif

    }

    std::string getAssetPath(const std::string& fileName) {
        return (getProjectRoot() / "assets" / fileName).string();
    }

    std::string getShaderPath(const std::string& fileName) {
        return (getProjectRoot() / "shaders" / fileName).string();
    }

    std::string getSPVPath(const std::string& fileName) {
        return (getProjectRoot() / "spv" / fileName).string();
    }
}