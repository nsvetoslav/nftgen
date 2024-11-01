#include <iostream>
#include "filemanager.hpp"

namespace nftgen {

	class generator {
	public:
		generator(std::string traitsDirectory)
			: _traitsDirectory(traitsDirectory)
		{

		}

		virtual ~generator() = default;

	public:
		void generate() {
			utilities::filemanager fileManager(_traitsDirectory);

			try
			{
				fileManager.throwIfNotExists();

			}
			catch (const auto& exception)
			{
				// logger.log(exception); 
			}
		}

	private:
		std::string _traitsDirectory;
	};
}
