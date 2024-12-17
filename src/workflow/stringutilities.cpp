#include "stringutilities.hpp"

namespace nftgen
{
namespace utilities
{

std::string stringutilities::clean_prefix(const std::string &input)
{
    return std::string();
}

std::string stringutilities::clean_postfix(const std::string &input)
{
    std::string result = input;

    trim(result);

    size_t last_dot = result.rfind('.');
    if (last_dot != std::string::npos)
    {
        result.erase(last_dot);
    }

    return result;
}

void stringutilities::trim(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));

    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), str.end());
}

}  // namespace utilities
}  // namespace nftgen
