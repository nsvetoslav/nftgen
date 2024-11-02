#include <vector>
#include "src/models/trait.hpp"

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

			double totalChance = chance * count;
			double adjustment = 1.0 - totalChance;

			if (count > 0) 
			{
				double lastChance = traits.back().getGenerationChance() + adjustment;
				traits.back().setGenerationChance(lastChance);
			}
		}
	};
}
