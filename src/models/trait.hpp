#include <string>
#include <vector>

class Trait
{
public:
    Trait(std::string path)
        : _generationChance(0.0),
          _isExcluded(false),
        _path(path)
    {
    }

    virtual ~Trait() = default;

private:
    std::string _path;
    double _generationChance;
    bool _isExcluded;
};