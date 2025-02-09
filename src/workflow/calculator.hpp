#pragma once

#include <vector>
#include "../models/trait.hpp"

namespace nftgen
{
class calculator
{
public:
    static void SetEqualGenerationChances(std::vector<Trait> &traits)
    {
        size_t count = traits.size();
        if (count == 0)
            return;

        // Calculate the total defined generation chance
        double totalDefinedChance = 0.0;
        for (const auto &trait : traits)
        {
            totalDefinedChance += trait.GetGenerationChance();
        }

        // Ensure the total defined chance does not exceed 1
        if (totalDefinedChance > 1.0)
        {
            throw std::runtime_error("Total defined generation chance exceeds 100%.");
        }

        // Calculate remaining chance to be equally distributed
        double remainingChance = 1.0 - totalDefinedChance;

        // Count traits with undefined (0) generation chance
        size_t undefinedCount = 0;
        for (const auto &trait : traits)
        {
            if (trait.GetGenerationChance() == 0.0)
            {
                ++undefinedCount;
            }
        }

        // If no traits need updating, return early
        if (undefinedCount == 0)
            return;

        // Distribute remaining chance equally among traits with undefined chance
        double equalChance = remainingChance / undefinedCount;
        for (auto &trait : traits)
        {
            if (trait.GetGenerationChance() == 0.0)
            {
                trait.SetGenerationChance(equalChance);
            }
        }
    }

    static void SetFullGenerationChances(std::vector<TraitDirectory> &traitsDirectories)
    {
        for (auto &traitDirectory : traitsDirectories)
        {
            if (traitDirectory.GetGenerationChance() == 0.00)
                traitDirectory.SetGenerationChance(1);
        }
    }

    static void SetEqualGenerationChances(std::vector<TraitDirectory> &traitsDirectories)
    {
        size_t count = traitsDirectories.size();
        if (count == 0)
            return;

        double chance = 1.0 / count;

        for (auto &traitDirectory : traitsDirectories)
        {
            if (traitDirectory.GetGenerationChance() == 0.00)
                traitDirectory.SetGenerationChance(chance);
        }
    }
};
}  // namespace nftgen
