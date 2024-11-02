#include <string>
#include <vector>

#include "trait.hpp"

class TraitFolder
{
public:
    TraitFolder(std::string path)
        : _generationChance(0.0),
        _path(path)
    {
    }

    virtual ~TraitFolder() = default;

public:
    void setPath(std::string path)
    {
        _path = path;
    }

    void setTraits(std::vector<std::string>& traits) {
        for (const auto& traitPath : traits)
			_traits.push_back(Trait(traitPath));
    }

private:
    std::string _path;
    std::vector<Trait> _traits;
    double _generationChance;
};
