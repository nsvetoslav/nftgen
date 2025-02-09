#pragma once

#include <string>
#include <vector>

#include "trait.hpp"
#include "exceptions.hpp"
#include "trait.hpp"

namespace nftgen
{
struct TraitDirectory
{
public:
    void SetPath(std::string path);
    [[nodiscard]] std::string_view GetPath();

    [[nodiscard]] std::string GetDirectoryName();
    void SetDirectoryName(std::string directory_name);

    void SetTraits(std::vector<std::pair<std::string /*path*/, std::string> /*file_name*/> traits,
                   std::string directoryName,
                   int &directoryGenerationID,
                   int &directoryID);

    [[nodiscard]] std::vector<nftgen::Trait> &GetTraits();

    [[nodiscard]] const int &GetID() const;
    void SetID(int id);

    [[nodiscard]] const double GetGenerationChance() const;
    void SetGenerationChance(double generationChance);

    [[nodiscard]] int GetGenerationOrder() const;
    void SetGenerationOrder(int generationOrder);

private:
    std::string _path;
    std::string _directoryName;
    std::vector<nftgen::Trait> _traits{};
    double _generationChance{};
    int _generationOrder{};
    int _id{};
};

}  // namespace nftgen
