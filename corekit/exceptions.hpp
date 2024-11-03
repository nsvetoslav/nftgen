#pragma once

#include <iostream>

namespace core {
	namespace file_exceptions {
		class NotExistingFileException : public std::runtime_error {
		public:
			explicit NotExistingFileException(const char* _Message)
				: std::runtime_error(_Message) {}
		};
	}
}