#include "src/workflow/generator.hpp"
#include "src/workflow/settings.hpp"

int main() {
	if (!nftgen::settings::getInstance().load())
		return -1;

	nftgen::generator generator(nftgen::settings::getInstance().get_assets_directory());

	auto start = std::chrono::high_resolution_clock::now();
	generator.generate();
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Elapsed time: " << duration.count() << " seconds." << std::endl;

	return 1;
}
