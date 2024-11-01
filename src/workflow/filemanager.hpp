#include <filesystem>
#include <iostream>
#include <stdexcept>
#include "corekit/exceptions.hpp"

namespace utilities
{
	class filemanager
	{
	public:
		filemanager(const std::string filename)
			: _filename(filename)
		{

		}

		virtual ~filemanager() = default;

	public:
		void throwIfNotExists() 
		{
			try 
			{
				if (!std::filesystem::exists(_filename))
				{
					throw core::NotExistingFileException(_filename);
				}
			}
			catch (std::exception ex)
			{
				throw ex;
			}
		}

		void openFile()
		{

		}

		void closeFile() 
		{

		}

		void createFile()
		{

		}

		void saveFile() 
		{

		}

		void deleteFile() 
		{

		}

	private:
		std::string _filename;
	};
}
