#include "src/workflow/generator.hpp"
#include "src/workflow/settings.hpp"

int main() {
	if (!nftgen::settings::getInstance().load())
		return -1;

	nftgen::generator generator(nftgen::settings::getInstance().get_assets_directory());

	return generator.generate();
}
