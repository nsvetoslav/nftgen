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
    static settings &get_instance [[nodiscard]] ();

private:
    settings() = default;

public:
    bool load();
    std::string get_assets_directory [[nodiscard]] ();
    std::string get_working_directory [[nodiscard]] ();
    std::string get_generated_nfts_directory [[nodiscard]] ();
    std::string get_template_file [[nodiscard]] ();

private:
    static std::string _settings_path;
    static json _settings_data;
};
}  // namespace nftgen
