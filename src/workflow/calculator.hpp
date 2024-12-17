#pragma once

#include <vector>
#include "../models/trait.hpp"

namespace nftgen
{
class calculator
{
  public:
    static void set_equal_geneartion_chances(std::vector<Trait> &traits)
    {
        size_t count = traits.size();
        if (count == 0)
            return;

        double chance = 1.0 / count;

        for (auto &trait : traits)
        {
            if (trait.get_generation_chance() == 0.00)
                trait.set_generation_chance(chance);
        }
    }

    static void reapply_generation_chances_by_exception()
    {
    }

    static void set_full_generation_chances(std::vector<TraitDirectory> &traits_directories)
    {
        for (auto &trait_directory : traits_directories)
        {
            if (trait_directory.get_generation_chance() == 0.00)
                trait_directory.set_generation_chance(1);
        }
    }

    static void set_equal_geneartion_chances(std::vector<TraitDirectory> &traits_directories)
    {
        size_t count = traits_directories.size();
        if (count == 0)
            return;

        double chance = 1.0 / count;

        for (auto &trait_directory : traits_directories)
        {
            if (trait_directory.get_generation_chance() == 0.00)
                trait_directory.set_generation_chance(chance);
        }
    }
};
}  // namespace nftgen
