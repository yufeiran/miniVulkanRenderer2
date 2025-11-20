#include <string>
#include <filesystem>

namespace mini {
    inline std::filesystem::path getProjectRoot(){
        #ifdef _WIN32 
            auto exePath = std::filesystem::current_path();
            return exePath.parent_path().parent_path();
        #else
            auto exePath = std::filesystem::current_path();
            return exePath.parent_path();
        #endif

    }

    inline std::string getAssetPath(const std::string&fileName){
        return (getProjectRoot() / "assets" / fileName).string();
    }

    inline std::string getShaderPath(const std::string&fileName){
        return (getProjectRoot() / "shaders" / fileName).string();
    }

    inline std::string getSPVPath(const std::string&fileName){
        return (getProjectRoot() / "spv" / fileName).string();
    }
}