#include "trait.h"
#include <chrono>
#include <numeric>
#include <optional>
#include <random>
#include "../workflow/generator.hpp"

namespace nftgen
{
void Trait::SetGenerationChance(double generationChance)
{
    _generationChance = generationChance;
}

[[nodiscard]] const double Trait::GetGenerationChance() const
{
    return _generationChance;
}

void Trait::SetDirectoryGenerationOrderID(int id)
{
    _directoryGenerationOrderID = id;
}

void Trait::SetDirectoryID(int id)
{
    _directoryID = id;
}

[[nodiscard]] const int Trait::GetTraitID() const
{
    return _traitID;
}

[[nodiscard]] const cv::Mat &Trait::GetMatrix() const
{
    return _imageMatrix;
}

std::pair<Trait *, GenerationResult> Trait::GetNextTrait(int directoryID, bool doNotApplyGenerationChances)
{
    auto &trait_directory = generator::_traitsDirectories[directoryID];
    auto &current_folder_traits = trait_directory.GetTraits();

    // start check if gen chance for folder
    std::random_device rd_trait_directory;
    std::mt19937 mt_gen_trait_directory(rd_trait_directory());
    std::uniform_real_distribution<float> uniform_dis_trait_directory(0.0f, 1.0f);

    float rand_value_trait_folder = uniform_dis_trait_directory(mt_gen_trait_directory);
    if (rand_value_trait_folder > trait_directory.GetGenerationChance() && !doNotApplyGenerationChances)
    {
        // std::cout << "Skipping directory ID: " << trait_directory_id << std::endl;
        return std::make_pair(nullptr, SkippedFolder);
    }
    // end check if gen chance for folder

    // start check if gen chance for traits
    std::random_device rd_trait;
    std::mt19937 mt_gen_trait(rd_trait());
    std::uniform_real_distribution<float> uniform_dis_trait(0.0f, 1.0f);

    float random_value = uniform_dis_trait(mt_gen_trait);

    Trait *next_trait = nullptr;

    float cumulative_probability = 0.0f;
    for (size_t i = 0; i < current_folder_traits.size(); ++i)
    {
        cumulative_probability += current_folder_traits[i].GetGenerationChance();
        if (random_value <= cumulative_probability)
        {
            next_trait = &current_folder_traits[i];
            break;
        }
    }
    // end check if gen chance for traits

    return std::make_pair(next_trait, SuccessfullyGenerated);
}

[[nodiscard]] Rarities Trait::GetRarity() const
{
    return _rarity;
}

[[nodiscard]] int Trait::GetDirectoryGenerationOrderID() const
{
    return _directoryGenerationOrderID;
}

[[nodiscard]] int Trait::GetDirectoryID() const
{
    return _directoryID;
}

void Trait::SetTraitID(int trait_id)
{
    _traitID = trait_id;
}

void Trait::SetMatrix(cv::Mat &image_matrix)
{
    _imageMatrix = image_matrix;
}

void Trait::SetRarity(const Rarities rarity)
{
    _rarity = rarity;
}

void Trait::SetPath(const std::string &path)
{
    _path = path;
}

void Trait::SetFilename(const std::string &filename)
{
    _filename = filename;
}

void Trait::SetDirectoryName(const std::string &directory_name)
{
    _directory_name = directory_name;
}

[[nodiscard]] std::string_view Trait::GetPath() const
{
    return _path;
}

[[nodiscard]] std::string_view Trait::GetFilename() const
{
    return _filename;
}

[[nodiscard]] std::string_view Trait::GetDirectoryName() const
{
    return _directory_name;
}
}  // namespace nftgen
