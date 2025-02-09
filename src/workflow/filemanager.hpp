#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace utilities
{
class filemanager
{
  public:
    filemanager()
    {
    }

    virtual ~filemanager() = default;

  public:
    void GetFilePathsInDirectory(const std::string &path, std::vector<std::pair<std::string, std::string>> &files) const
    {
        for (const auto &entry : std::filesystem::directory_iterator(path))
            files.push_back(std::make_pair(entry.path().string(), entry.path().filename().string()));
    }
};
}  // namespace utilities
