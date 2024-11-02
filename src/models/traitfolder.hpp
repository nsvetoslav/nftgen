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

    void setTraits(std::vector<std::string>& traits);

    inline std::vector<Trait>& getTraits()
    {
        return _traits;
    }

    inline std::string_view get_path [[nodiscard]]() {
	return _path;
    }
    int id;

private:
    std::string _path;
    std::vector<Trait> _traits{};
    double _generationChance{};
};
