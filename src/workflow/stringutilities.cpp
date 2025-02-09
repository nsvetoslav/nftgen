#include "stringutilities.hpp"
#include <regex>

namespace nftgen
{
namespace utilities
{

std::string stringutilities::ClearPrefix(const std::string &input)
{
    std::string name = input;

    size_t pos = input.find('.');

    if (pos != std::string::npos)
    {
        name = name.substr(pos + 1);
    }

    return name;
}

std::string stringutilities::ClearPostfix(const std::string &input)
{
    std::string result = input;

    Trim(result);

    size_t lastDot = result.rfind('.');
    if (lastDot != std::string::npos)
    {
        result.erase(lastDot);
    }

    return result;
}

void stringutilities::Trim(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));

    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), str.end());
}

}  // namespace utilities
}  // namespace nftgen
