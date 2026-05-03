#pragma once
#include <string>
#include <filesystem>

namespace mini {
    std::filesystem::path getProjectRoot();

    std::string getAssetPath(const std::string& fileName);

    std::string getShaderPath(const std::string& fileName);

    std::string getSPVPath(const std::string& fileName);
}