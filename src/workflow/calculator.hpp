#pragma once

#include <vector>
#include "../models/trait.hpp"

namespace nftgen
{
class calculator
{
  public:
    static void set_equal_generation_chances(std::vector<Trait>& traits)
    {
        size_t count = traits.size();
        if (count == 0)
            return;

        // Calculate the total defined generation chance
        double total_defined_chance = 0.0;
        for (const auto &trait : traits)
        {
            total_defined_chance += trait.get_generation_chance();
        }

        // Ensure the total defined chance does not exceed 1
        if (total_defined_chance > 1.0)
        {
            throw std::runtime_error("Total defined generation chance exceeds 100%.");
        }

        // Calculate remaining chance to be equally distributed
        double remaining_chance = 1.0 - total_defined_chance;

        // Count traits with undefined (0) generation chance
        size_t undefined_count = 0;
        for (const auto &trait : traits)
        {
            if (trait.get_generation_chance() == 0.0)
            {
                ++undefined_count;
            }
        }

        // If no traits need updating, return early
        if (undefined_count == 0)
            return;

        // Distribute remaining chance equally among traits with undefined chance
        double equal_chance = remaining_chance / undefined_count;
        for (auto &trait : traits)
        {
            if (trait.get_generation_chance() == 0.0)
            {
                trait.set_generation_chance(equal_chance);
            }
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
