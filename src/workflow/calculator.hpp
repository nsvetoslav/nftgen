#pragma once

#include <vector>
#include "../models/trait.hpp"

namespace nftgen 
{
	class calculator 
	{
	public:
		static void setEqualGenerationChances(std::vector<Trait>& traits) 
		{
			size_t count = traits.size();
			if (count == 0)
				return;

			double chance = 1.0 / count;

			for (auto& trait : traits)
				trait.setGenerationChance(chance);
		}
	};
}
