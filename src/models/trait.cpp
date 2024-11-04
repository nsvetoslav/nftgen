#include "trait.hpp"
#include <chrono>
#include <optional>
#include "../workflow/generator.hpp"

using namespace nftgen;

std::optional<Trait> Trait::get_next_trait() const {
	if (_traitFolderId + 1 == generator::_traitsDirectories.size()) return std::nullopt;

	auto &traitDirectory = generator::_traitsDirectories[_traitFolderId + 1];
	auto &next_folder = traitDirectory.getTraits();

	std::vector<double> probs(next_folder.size());
	for (int i = 0; i < probs.size(); i++) { probs[i] = next_folder[i].get_generation_chance(); }

	auto time = std::chrono::system_clock::now();
	std::srand(get_unix_time());

	int rand_next = std::rand() % next_folder.size();

	Trait nextTrait = next_folder[rand_next];

	nextTrait.set_trait_id(rand_next);

	return nextTrait;
}
