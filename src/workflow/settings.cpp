#include "settings.hpp"

namespace nftgen
{

std::string settings::_settingsPath = "settings.json";
json settings::_settingsData;

settings &settings::GetInstance [[nodiscard]] ()
{
    static settings instance;
    return instance;
}

bool settings::load()
{
    std::ifstream inputFile(_settingsPath);
    if (!inputFile.is_open())
    {
        std::cerr << "Could not open the settings file: " << _settingsPath << std::endl;
        return false;
    }

    try
    {
        inputFile >> _settingsData;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }

    inputFile.close();
    return true;
}

std::string settings::GetAssetsDirectory [[nodiscard]] ()
{
    return _settingsData.value("assets_directory", "");
}

std::string settings::GetWorkingDirectory [[nodiscard]] ()
{
    return _settingsData.value("working_directory", "");
}

std::string settings::GetGeneratedNFTsDirectory [[nodiscard]] ()
{
    return _settingsData.value("generated_nfts_directory", "");
}

std::string settings::GetTemplateFile [[nodiscard]] ()
{
    return _settingsData.value("nfts_template_directory", "");
}
}  // namespace nftgen
