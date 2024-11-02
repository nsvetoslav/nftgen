#include "generator.hpp"
#include <format>

using namespace nftgen;

std::vector<TraitFolder> generator::_traitsDirectories{};

bool generator::loadDirectories()
{
	std::string root_path = _traitsDirectory;
	utilities::filemanager fileManager;

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

	return true;
}


bool generator::generate() {
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

		std::optional<Trait> first_trait = generate_first_random_trait();
		int i{};
		do {
			std::cout << i << "asset chosen : " << first_trait->get_path() << std::endl;
			first_trait = first_trait->get_next_trait();
			i++;
		} while(first_trait.has_value());

		// Setting generation chances for the first prioritized traitDirectory
		setGenerationChacnes(_traitsDirectories.at(0));
	}
	catch (const std::exception& exception)
	{
		// logger.log(exception); 
		return false;
	}

	return true;
}
		

std::optional<Trait> generator::generate_first_random_trait [[nodiscard]] () {
	std::sort(_traitsDirectories.begin(), _traitsDirectories.end(), [](auto &dir1, auto &dir2){
		return dir1.get_path() < dir2.get_path();
	});
	for(int i = 0; i < _traitsDirectories.size(); i++) {
		_traitsDirectories[i].id = i;
		for(auto &trait : _traitsDirectories[i].getTraits()) {
			trait.traitFolderId = i;
		}
	}
	auto time = Trait::get_unix_time();
	std::srand(time);
	return _traitsDirectories[0].getTraits()[std::rand() % _traitsDirectories[0].getTraits().size()];
}
