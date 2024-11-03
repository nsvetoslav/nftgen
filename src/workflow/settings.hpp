#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace nftgen {
class settings {
public:
	settings(const settings&) = delete;
	settings& operator=(const settings&) = delete;

	static settings& get_instance() {
		static settings instance;
		return instance;
	}

private:
	settings() = default;

public:
	bool load();  
	std::string get_assets_directory();
	std::string get_working_directory();
	std::string get_generated_nfts_directory();

private:
	static std::string _settingsPath;
	static json _settingsData;
};
}