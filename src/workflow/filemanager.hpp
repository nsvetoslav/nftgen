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

		void getDirectoryFilePaths(const std::string& path, std::vector<std::string>& files) const
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
				files.push_back(entry.path().string()); 
		}

		void openFile()
		{
		}

		void closeFile()
		{
		}

		//void createFile()
		//{
		//	std::ofstream file(_filename);
		//	if (!file) {
		//		throw std::runtime_error("Could not create file: " + _filename);
		//	}
		//}

		//void saveFile(const std::string& content)
		//{
		//	std::ofstream file(_filename);
		//	if (!file) {
		//		throw std::runtime_error("Could not open file for saving: " + _filename);
		//	}
		//	file << content;
		//}

		//void deleteFile()
		//{
		//	if (std::filesystem::remove(_filename)) {
		//		std::cout << "File deleted successfully: " << _filename << std::endl;
		//	}
		//	else {
		//		std::cerr << "Failed to delete file: " << _filename << std::endl;
		//	}
		//}
	};
}
