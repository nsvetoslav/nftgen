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

void TraitDirectory::set_traits(std::vector<std::pair<std::string /*path*/, std::string> /*file_name*/> traits,
                                std::string direcotry_name,
                                int &gen_trait_directory_id,
                                int &directory_index)
{
    for (int trait_id = 0; trait_id < traits.size(); ++trait_id)
    {
        const auto &trait_path = traits[trait_id];

        cv::Mat&& image_matrix = cv::imread(trait_path.first, cv::IMREAD_UNCHANGED);
        Trait trait;
        trait.set_path(trait_path.first);
        trait.set_filename(nftgen::utilities::stringutilities::clean_postfix(trait_path.second));
        trait.set_directory_name(nftgen::utilities::stringutilities::clean_prefix(direcotry_name));
        trait.set_gen_order_trait_directory_id(gen_trait_directory_id);
        trait.set_trait_id(trait_id);
        trait.set_trait_directory_id(directory_index);
        trait.set_matrix(image_matrix);
        //-Ranbow Mouth Should be generated less
        //-"Portal Eyes" Should be generated less
        //-"7 Eyes" Should be generated less
        //-"Chip Eyes" Should be generated less
        //-"Flame Horns" should be generated only with darker backgrounds
        //-"Card suit eyes" (spades, hearts, diamonds, spades) should be genereted less

        if (trait.get_filename().find("Rainbow Mouth") != std::string::npos)
            trait.set_generation_chance(0.002);

        else if (trait.get_filename().find("Portal Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("77 Eyes") != std::string::npos)
            trait.set_generation_chance(0.0008);

        else if (trait.get_filename().find("Chip Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Flame Horns") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Spade Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Hearts Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Heart Eyes V1") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Heart Eyes V2") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Diamond Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Spade Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        else if (trait.get_filename().find("Exhale Mouth") != std::string::npos)
            trait.set_generation_chance(0.002);

        else if (trait.get_filename().find("Spade Eyes") != std::string::npos)
            trait.set_generation_chance(0.001);

        _traits.push_back(trait);
    }

    nftgen::calculator::set_equal_generation_chances(_traits);
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

int TraitDirectory::get_gen_order() const
{
    return _generation_order;
}

void TraitDirectory::set_gen_order(int gen_order)
{
    _generation_order = gen_order;
}

std::string_view TraitDirectory::get_path [[nodiscard]] ()
{
    return _path;
}

std::string TraitDirectory::get_directory_name [[nodiscard]] ()
{
    return _directory_name;
}

}  // namespace nftgen
