//- If "Hood" is generated you cannot generate "Horns" or "Top Acc".
//- If "Rainbow Mouth" is generated it is generated at the end thus no "Extras" are generated after.
//- If "Beard" is generated it is placed in the 3rd position right before "Eyes" and "Mouths".
//- If "Top Acc" is generated no "Horns" are generated.
//- If any variation of the "Cap" Trait is generated no "Earings" or "Clothes" with back pieces can be generated.
//- Extras should be rarely generated on some of the NFT's (the lowest rate of generated traits)
//- Earings should be rarely generated on some of the NFT's (the lowest rate of generated traits)
//- Gold Horns are only generated with Gold Skin
//- Silver Horns are only generated with Silver Skin
//- Ranbow Mouth Should be generated less
//- "Black" skin should be removed and not generated because it causes too much problems and if traits are tailored for it, they dont look good on the other ones.
//- "Portal Eyes" Should be generated less
//- "7 Eyes" Should be generated less
//- "Chip Eyes" Should be generated less
//- "Flame Horns" should be generated only with darker backgrounds
//- "Card suit eyes" (spades, hearts, diamonds, spades) should be genereted less
//- "Hear Eyes v2" should be generated less
//- Any type of "Wood Horns" should not be generated with "Brown Skin"
//- "Rainbow Mouth" should not be genereted with "Old Time Slot Machine"
//- "Exhale Mouth" should be generated less
//- "Red Skin" should not be genereted with "Red Kimono"
//- "Blush Mouth" Should not be genereted with "Silver Skin"
//- "Anime Eyes" should not be genereted with "Tie" and "Headband" traits
//- "Anime Eyes" Should be generated with the same color skin
//- "Golden Glasses" should be generated more with "Scar Eye"
//- "Beanie" trait should only be generated with simple clothing(Stripped Shirt, Tank Top, Hoodie.Tee, )
//- If "Hood" is generated you cannot generate "Horns" or "Top Acc".
//- If "Rainbow Mouth" is generated it is generated at the end thus no "Extras" are generated after.
//- If "Beard" is generated it is placed in the 3rd position right before "Eyes" and "Mouths".
//- If "Top Acc" is generated no "Horns" are generated.
//- If any variation of the "Cap" Trait is generated no "Earings" or "Clothes" with back pieces can be generated.
//- Extras should be rarely generated on some of the NFT's (the lowest rate of generated traits)

#include <iostream>
#include "src/workflow/generator.hpp"

int main()
{
	nftgen::generator generator("../../../../assets");
	generator.generate();

	return -1;
}