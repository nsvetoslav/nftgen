#include "traitfolder.hpp"

void TraitFolder::setTraits(std::vector<std::string> &traits) {
    for (const auto &traitPath : traits) _traits.push_back(Trait(traitPath));
}
