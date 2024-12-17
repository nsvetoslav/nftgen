#include "traitfolder.hpp"
#include "../workflow/calculator.hpp"
#include "../workflow/stringutilities.hpp"

namespace nftgen
{

void TraitDirectory::set_path(std::string path)
{
    _path = path;
}

void TraitDirectory::set_directory_name(std::string directory_name)
{
    _directory_name = directory_name;
}

void TraitDirectory::set_traits(std::vector<std::pair<std::string /*path*/, std::string> /*file_name*/> traits, std::string direcotry_name, int &trait_directory_id)
{
    for (int trait_id = 0; trait_id < traits.size(); ++trait_id)
    {
        const auto &trait_path = traits[trait_id];

        Trait trait;
        trait.set_path(trait_path.first);
        trait.set_filename(nftgen::utilities::stringutilities::clean_postfix(trait_path.second));
        trait.set_directory_name(nftgen::utilities::stringutilities::clean_postfix(direcotry_name));
        trait.set_trait_directory_id(trait_directory_id);
        trait.set_trait_id(trait_id);

        const cv::Mat image_matrix = cv::imread(trait_path.first, cv::IMREAD_UNCHANGED);
        trait.set_matrix(image_matrix);

        _traits.push_back(trait);
    }

    nftgen::calculator::set_equal_geneartion_chances(_traits);
}

const int &TraitDirectory::get_id [[nodiscard]] () const
{
    return _id;
}

void TraitDirectory::set_id(int id)
{
    _id = id;
}

std::vector<nftgen::Trait> &TraitDirectory::get_traits [[nodiscard]] ()
{
    return _traits;
}

const double TraitDirectory::get_generation_chance [[nodiscard]] () const
{
    return _generation_chance;
}

void TraitDirectory::set_generation_chance(double generation_chance)
{
    _generation_chance = generation_chance;
}

std::string_view TraitDirectory::get_path [[nodiscard]] ()
{
    return _path;
}

std::string TraitDirectory::get_directory_name [[nodiscard]] ()
{
    return _directory_name;
}

void TraitDirectory::add_exception(int traitFolderId, nftgen::ExceptionsTypes exceptionType)
{
    _exceptions.emplace_back(TraitFolderException(traitFolderId, exceptionType));
}

}  // namespace nftgen
