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