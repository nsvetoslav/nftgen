#include "settings.hpp"

namespace nftgen {
std::string settings::_settingsPath = "settings.json";
json settings::_settingsData; 

bool settings::load() {
	std::ifstream inputFile(_settingsPath);
	if (!inputFile.is_open()) {
		std::cerr << "Could not open the settings file: " << _settingsPath << std::endl;
		return false;
	}

	try {
		inputFile >> _settingsData;
	}
	catch (const json::parse_error& e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
		return false;
	}

	inputFile.close();
	return true;
}

std::string settings::get_assets_directory() {
	return _settingsData.value("assets_directory", ""); 
}

std::string settings::get_working_directory() {
	return _settingsData.value("working_directory", ""); 
}

std::string settings::get_generated_nfts_directory() {
	return _settingsData.value("generated_nfts_directory", ""); 
}
}