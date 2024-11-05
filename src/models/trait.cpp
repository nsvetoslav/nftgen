#include "trait.hpp"
#include <chrono>
#include <numeric>
#include <optional>
#include <random>
#include "../workflow/generator.hpp"

using namespace nftgen;

std::optional<Trait> Trait::get_next_trait() const {
	if (_traitFolderId + 1 == generator::_traitsDirectories.size()) return std::nullopt;

	auto &traitDirectory = generator::_traitsDirectories[_traitFolderId + 1];
	auto &current_folder_traits = traitDirectory.getTraits();

	std::random_device					  rd;
	std::mt19937						  gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	float randValue = dis(gen);

	std::optional<Trait> nextTrait = std::nullopt;

	float cumulativeProbability = 0.0f;
	for (size_t i = 0; i < current_folder_traits.size(); ++i) {
		cumulativeProbability += current_folder_traits[i].get_generation_chance();
		if (randValue <= cumulativeProbability) {
			nextTrait = current_folder_traits[i];
			nextTrait->set_trait_id(i);
			nextTrait->set_trait_folder_id(traitDirectory.getId());
			break;
		}
	}

	return nextTrait;
}
