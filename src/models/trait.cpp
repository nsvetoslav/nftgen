#include "trait.hpp"
#include <chrono>
#include <numeric>
#include <optional>
#include <random>
#include "../workflow/generator.hpp"

namespace nftgen
{
void Trait::set_generation_chance(double &generationChance)
{
    _generation_chance = generationChance;
}

const double Trait::get_generation_chance [[nodiscard]] () const
{
    return _generation_chance;
}

void Trait::set_trait_directory_id(int id)
{
    _trait_folder_id = id;
}

const int Trait::get_trait_id [[nodiscard]] () const
{
    return _trait_id;
}

const cv::Mat &Trait::get_matrix [[nodiscard]] () const
{
    return _image_matrix;
}

std::pair<std::optional<Trait>, GenerationResult> Trait::get_next_trait(int trait_directory_id)
{
    auto &trait_directory = generator::_traits_directories[trait_directory_id];
    auto &current_folder_traits = trait_directory.get_traits();

    // start check if gen chance for folder
    std::random_device rd_trait_directory;
    std::mt19937 mt_gen_trait_directory(rd_trait_directory());
    std::uniform_real_distribution<float> uniform_dis_trait_directory(0.0f, 1.0f);

    float rand_value_trait_folder = uniform_dis_trait_directory(mt_gen_trait_directory);
    if (rand_value_trait_folder > trait_directory.get_generation_chance())
    {
        std::cout << "Skipping directory ID: " << trait_directory_id << std::endl;
        return std::make_pair(std::nullopt, SkippedFolder);
    }
    // end check if gen chance for folder

    // start check if gen chance for traits
    std::random_device rd_trait;
    std::mt19937 mt_gen_trait(rd_trait());
    std::uniform_real_distribution<float> uniform_dis_trait(0.0f, 1.0f);

    float random_value = uniform_dis_trait(mt_gen_trait);

    std::optional<Trait> next_trait = std::nullopt;

    float cumulative_probability = 0.0f;
    for (size_t i = 0; i < current_folder_traits.size(); ++i)
    {
        cumulative_probability += current_folder_traits[i].get_generation_chance();
        if (random_value <= cumulative_probability)
        {
            next_trait = current_folder_traits[i];
            break;
        }
    }
    // end check if gen chance for traits

    return std::make_pair(next_trait, SuccessfullyGenerated);
}

Rarities Trait::get_rarity [[nodiscard]] () const
{
    return _rarity;
}

int Trait::get_trait_directory_id [[nodiscard]] () const
{
    return _trait_folder_id;
}

void Trait::set_trait_id(int trait_id)
{
    _trait_id = trait_id;
}

void Trait::set_matrix(const cv::Mat &image_matrix)
{
    _image_matrix = image_matrix;
}

void Trait::set_rarity(const Rarities rarity)
{
    _rarity = rarity;
}

void Trait::set_path(const std::string &path)
{
    _path = path;
}

void Trait::set_filename(const std::string &filename)
{
    _filename = filename;
}

void Trait::set_directory_name(const std::string &directory_name)
{
    _directory_name = directory_name;
}

//bool Trait::meets_all_exceptions(const Exceptions &exceptions)
//{
//    return true;
//}

std::string_view Trait::get_path [[nodiscard]] () const
{
    return _path;
}

std::string_view Trait::get_filename [[nodiscard]] () const
{
    return _filename;
}

std::string_view Trait::get_directory_name [[nodiscard]] () const
{
    return _directory_name;
}
}  // namespace nftgen
