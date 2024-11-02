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
		bool generate() {

			try
			{
				if (!loadDirectories())
				{
					return false;
				}
				
				if (_traitsDirectories.empty())
				{
					return false;
				}

				// Setting generation chances for the first prioritized traitDirectory
				setGenerationChacnes(_traitsDirectories.front());

			}
			catch (const std::exception& exception)
			{
				// logger.log(exception); 
				return false;
			}

			return true;
		}

	private:

		bool loadDirectories()
		{
			std::string root_path = _traitsDirectory;
			utilities::filemanager fileManager;

			if (!std::filesystem::exists(root_path) && !std::filesystem::is_directory(root_path))
			{
				// logger.log(!fileManager.exists(), _traitsDirectory);
				return false;
			}

			// Traversing directories in assets
			std::vector<std::string> directoryPaths;
			fileManager.getDirectoryFilePaths(root_path, directoryPaths);

			for (const auto& traitDirectory : directoryPaths) {
				TraitFolder traitFolder(traitDirectory);

				// All the trait files paths in a traitDirectory
				std::vector<std::string> traits;
				fileManager.getDirectoryFilePaths(traitDirectory, traits);

				traitFolder.setTraits(traits);

				// mem moving traitFolder in _traitsDirectories
				_traitsDirectories.push_back(std::move(traitFolder));
			}

			return true;
		}

		void setGenerationChacnes(TraitFolder& traitFolder)
		{
			nftgen::calculator calculator;
			calculator.setEqualGenerationChances(traitFolder.getTraits());
		}

	private:
		std::string _traitsDirectory;
		std::vector<TraitFolder> _traitsDirectories;
	};
}
