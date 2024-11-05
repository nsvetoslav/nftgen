#pragma once

#include <vector>
#include "../models/trait.hpp"

namespace nftgen {
	class calculator {
	public:
		static void set_equal_geneartion_chances(std::vector<Trait> &traits) {
			size_t count = traits.size();
			if (count == 0) return;

			double chance = 1.0 / count;

			for (auto &trait : traits) {
				if (trait.get_generation_chance() == 0.00) trait.set_generation_chance(chance);
			}
		}

		static void reapply_generation_chances_by_exception() {}

		static void set_equal_geneartion_chances(std::vector<TraitFolder> &traitsFolders) {
			size_t count = traitsFolders.size();
			if (count == 0) return;

			double chance = 1.0 / count;

			for (auto &traitFolder : traitsFolders) {
				if (traitFolder.get_generation_chance() == 0.00) traitFolder.set_generation_chance(chance);
			}
		}
	};
} // namespace nftgen
