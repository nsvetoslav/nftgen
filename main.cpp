#include <iostream>
#include "src/workflow/generator.hpp"

int main()
{
	nftgen::generator generator("assets");
	generator.generate();

	return -1;
}
