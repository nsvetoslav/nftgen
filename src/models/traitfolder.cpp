#include "traitfolder.hpp"
#include "../workflow/stringutilities.hpp"

void TraitFolder::setTraits(std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traits,
							std::string															   direcotry_name) {
	for (const auto &traitPath : traits) {
		_traits.push_back(Trait(traitPath.first,
								nftgen::utilities::stringutilities::clean_postfix(traitPath.second),
								nftgen::utilities::stringutilities::clean_prefix(direcotry_name)));
	}
}
