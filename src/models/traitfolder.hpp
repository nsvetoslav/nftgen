#include <string>
#include <vector>

#include "trait.hpp"

class TraitFolder
{
    TraitFolder()
        : _generationChance(0.0)
    {
    }

    virtual ~TraitFolder() = default;

private:
    std::string _path;
    std::vector<Trait> _traits;
    double _generationChance;
};
