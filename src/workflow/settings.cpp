#include "settings.hpp"

namespace nftgen
{

std::string settings::_settings_path = "settings.json";
json settings::_settings_data;

settings &settings::get_instance [[nodiscard]] ()
{
    static settings instance;
    return instance;
}

bool settings::load()
{
    std::ifstream inputFile(_settings_path);
    if (!inputFile.is_open())
    {
        std::cerr << "Could not open the settings file: " << _settings_path << std::endl;
        return false;
    }

    try
    {
        inputFile >> _settings_data;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }

    inputFile.close();
    return true;
}

std::string settings::get_assets_directory [[nodiscard]] ()
{
    return _settings_data.value("assets_directory", "");
}

std::string settings::get_working_directory [[nodiscard]] ()
{
    return _settings_data.value("working_directory", "");
}

std::string settings::get_generated_nfts_directory [[nodiscard]] ()
{
    return _settings_data.value("generated_nfts_directory", "");
}

std::string settings::get_template_file [[nodiscard]] ()
{
    return _settings_data.value("nfts_template_directory", "");
}
}  // namespace nftgen
