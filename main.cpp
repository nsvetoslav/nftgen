#include "src/workflow/generator.hpp"
#include "src/workflow/settings.hpp"

int main() {
	if (!nftgen::settings::get_instance().load()) return -1;

	nftgen::generator generator(nftgen::settings::get_instance().get_assets_directory());

	generator.generate(500);

	return 1;
}
