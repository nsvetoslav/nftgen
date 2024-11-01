#include <iostream>

namespace core {

	class NotExistingFileException : public std::runtime_error {
	public:
		explicit NotExistingFileException(const std::string& message)
			: std::runtime_error(message) {}
	};
}