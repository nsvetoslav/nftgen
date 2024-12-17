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
    static std::string clean_prefix(const std::string &input);
    static std::string clean_postfix(const std::string &input);
 
  private:
    static void trim(std::string &str);

};
}  // namespace utilities
}  // namespace nftgen
