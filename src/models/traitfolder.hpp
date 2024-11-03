#pragma once

#include <string>
#include <vector>

#include "trait.hpp"

class TraitFolder {
public:
    TraitFolder(std::string path) : _generationChance(0.0), _path(path) {}

    virtual ~TraitFolder() = default;

public:
    inline void setPath(std::string path) { _path = path; }

    void setTraits(std::vector<std::string> &traits);

    inline const int &getId() { return _id; }

    inline void setId(int id) { _id = id; }

    inline std::vector<Trait> &getTraits() { return _traits; }

    inline std::string_view get_path [[nodiscard]] () { return _path; }

private:
    std::string        _path;
    std::vector<Trait> _traits{};
    double             _generationChance{};
    int                _id{};
};
