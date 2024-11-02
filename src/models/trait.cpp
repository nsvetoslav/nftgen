#include "trait.hpp"
#include "../workflow/generator.hpp"
#include <optional>
#include <chrono>

using namespace nftgen;

std::optional<Trait> Trait::get_next_trait() const {
	if(traitFolderId + 1 == generator::_traitsDirectories.size())
		return std::nullopt;
	auto &next_folder = generator::_traitsDirectories[traitFolderId + 1].getTraits();

	std::vector<double> probs(next_folder.size());
	for(int i = 0; i < probs.size(); i++) {
		probs[i] = next_folder[i].getGenerationChance();
	}
	auto time = std::chrono::system_clock::now();
	std::srand(get_unix_time());

	int rand_next = std::rand() % next_folder.size();
	return next_folder[rand_next];
}


