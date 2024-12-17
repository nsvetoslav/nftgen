#include "src/workflow/generator.hpp"
#include "src/workflow/settings.hpp"

int main()
{
    if (!nftgen::settings::get_instance().load())
        return -1;

    nftgen::generator generator(nftgen::settings::get_instance().get_assets_directory());

    const auto& generation_result = generator.generate(200);

    return 1;
}
