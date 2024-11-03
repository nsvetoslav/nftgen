#pragma once

#include <filesystem>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>  
#include "../../corekit/exceptions.hpp" 
#include <string>

namespace utilities
{
	class filemanager
	{
	public:
		filemanager() 
		{}

		virtual ~filemanager() = default;

	public:

		void get_directory_file_paths(const std::string& path, std::vector<std::string>& files) const
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
				files.push_back(entry.path().string()); 
		}
	};
}
