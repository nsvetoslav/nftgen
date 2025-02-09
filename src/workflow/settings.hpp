#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

namespace nftgen
{
class settings
{
public:
    settings(const settings &) = delete;
    settings &operator=(const settings &) = delete;

public:
    static settings &GetInstance [[nodiscard]] ();

private:
    settings() = default;

public:
    bool load();
    std::string GetAssetsDirectory [[nodiscard]] ();
    std::string GetWorkingDirectory [[nodiscard]] ();
    std::string GetGeneratedNFTsDirectory [[nodiscard]] ();
    std::string GetTemplateFile [[nodiscard]] ();

private:
    static std::string _settingsPath;
    static json _settingsData;
};
}  // namespace nftgen
