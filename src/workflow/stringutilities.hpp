#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace nftgen
{
namespace utilities
{
class stringutilities
{
public:
    static std::string ClearPrefix(const std::string &input);
    static std::string ClearPostfix(const std::string &input);

private:
    static void Trim(std::string &str);
};
}  // namespace utilities
}  // namespace nftgen
