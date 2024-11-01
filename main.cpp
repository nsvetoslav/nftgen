#include <string>
#include <vector>

class Trait
{
public:
    Trait()
        : _generationChance(0.0),
          _isExcluded(false)
    {
    }

    virtual ~Trait() = default;

private:
    std::string _path;
    double _generationChance;
    bool _isExcluded;
};

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

class GenerationInput
{
public:
    GenerationInput() = default;
    virtual ~GenerationInput() = default;

private:
    std::vector<TraitFolder> _traitFolders;
};