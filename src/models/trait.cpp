#include "trait.hpp"
#include <chrono>
#include <optional>
#include "../workflow/generator.hpp"

using namespace nftgen;

std::optional<Trait> Trait::get_next_trait() const {
    if (_traitFolderId + 1 == generator::_traitsDirectories.size()) return std::nullopt;

    auto &next_folder = generator::_traitsDirectories[_traitFolderId + 1].getTraits();

    std::vector<double> probs(next_folder.size());
    for (int i = 0; i < probs.size(); i++) { probs[i] = next_folder[i].get_generation_chance(); }

    auto time = std::chrono::system_clock::now();
    std::srand(get_unix_time());

    int rand_next = std::rand() % next_folder.size();
    return next_folder[rand_next];
}
