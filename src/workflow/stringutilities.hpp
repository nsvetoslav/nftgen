#pragma once

#include <algorithm>
#include <cctype>
#include <string>
namespace nftgen {
	namespace utilities {
		class stringutilities {
		public:
			static std::string clean_prefix(const std::string &input) {
				std::string result = input;

				trim(result);

				size_t first_dot = result.find('.');
				if (first_dot != std::string::npos) { result.erase(0, first_dot + 1); }

				return result;
			}

			static std::string clean_postfix(const std::string &input) {
				std::string result = input;

				trim(result);

				size_t last_dot = result.rfind('.');
				if (last_dot != std::string::npos) {
					result.erase(last_dot); 
				}

				return result;
			}

		private:
			static void trim(std::string &str) {
				str.erase(str.begin(),
						  std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));

				str.erase(
					std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
					str.end());
			}
		};
	} // namespace utilities
} // namespace nftgen