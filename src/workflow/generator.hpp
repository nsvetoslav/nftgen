#include <iostream>
#include "filemanager.hpp"
#include "../models/traitfolder.hpp"

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
			std::string root_path = "../../../../assets";
			utilities::filemanager fileManager;

			try
			{
				if (!fileManager.exists(root_path) && !fileManager.isDirectory(root_path))
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
			}
			catch (const std::exception& exception)
			{
				// logger.log(exception); 
				return false;
			}

			return true;
		}

	private:
		std::string _traitsDirectory;
		std::vector<TraitFolder> _traitsDirectories;
	};
}
