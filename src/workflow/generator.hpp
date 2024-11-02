#include <iostream>
#include "filemanager.hpp"
#include "../models/traitfolder.hpp"
#include "calculator.hpp"

namespace nftgen {

	class generator {
	public:
		generator(std::string traitsDirectory)
			: _traitsDirectory(traitsDirectory)
		{

		}

		virtual ~generator() = default;

	public:
		bool generate();
		static std::vector<TraitFolder> _traitsDirectories;

	private:

		bool loadDirectories [[nodiscard]] ();

		void setGenerationChacnes(TraitFolder& traitFolder)
		{
			nftgen::calculator calculator;
			calculator.setEqualGenerationChances(traitFolder.getTraits());
		}
		std::string _traitsDirectory;

		std::optional<Trait> generate_first_random_trait [[nodiscard]] ();
	};
}
