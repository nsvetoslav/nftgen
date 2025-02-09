#include "generator.hpp"

#include <filesystem>
#include <format>

#include "generator.hpp"
#include "settings.hpp"
#include "../exceptions/factories.hpp"
#include "../exceptions/exception.hpp"
#include <memory>
#include <random>

#define ApplyFromDirectory(stringVal) stringVal
#define ApplyToTrait(stringVal) stringVal
#define ApplyFromTrait(stringVal) stringVal
#define ApplyFromTraitDirectory(stringVal) stringVal
#define ApplyToDirectory(stringVal) stringVal
#define NoTraitApply ""
#define GenerateOnlyWithTraits(val) val
#define RenderAfter(val) val

using namespace nftgen;

std::vector<TraitDirectory> generator::_traits_directories{};

[[nodiscard]] bool generator::load_directories()
{
    std::string root_path = _traits_directories_root_path;
    utilities::filemanager fileManager;

    if (!std::filesystem::exists(root_path) && !std::filesystem::is_directory(root_path))
    {
        return false;
    }

    // Traversing directories in assets
    std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> trait_directory_paths_pairs;
    fileManager.get_file_paths_in_directory(root_path, trait_directory_paths_pairs);

    for (int trait_directory_path_index = 0; trait_directory_path_index < trait_directory_paths_pairs.size(); ++trait_directory_path_index)
    {
        auto &trait_directory_path_pair = trait_directory_paths_pairs[trait_directory_path_index];
        const auto &trait_directory_path = trait_directory_path_pair.first;
        const auto &trait_directory_folder_name = trait_directory_path_pair.second;

        TraitDirectory trait_directory;
        trait_directory.set_path(trait_directory_path);
        trait_directory.set_directory_name(trait_directory_folder_name);
        trait_directory.set_id(trait_directory_path_index);

        if (trait_directory_folder_name == "6.Earings")
        {
            trait_directory.set_generation_chance(0.06);
        }
        // - Extras should be rarely generated on some of the NFT's (the lowest rate of generated traits
        if (trait_directory_folder_name == "9.Extras")
        {
            trait_directory.set_generation_chance(0.08);
        }
        if (trait_directory_folder_name == "Legacy")
        {
            trait_directory.set_generation_chance(0.03);
        }
        if (trait_directory_folder_name == "8.Top Acc")
        {
            trait_directory.set_generation_chance(0.42);
        }

        std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traits_paths;
        fileManager.get_file_paths_in_directory(trait_directory_path_pair.first, traits_paths);

        trait_directory.set_traits(traits_paths, trait_directory_folder_name, trait_directory_path_index, trait_directory_path_index);

        _traits_directories.push_back(trait_directory);
    }

    nftgen::calculator::set_full_generation_chances(_traits_directories);

    for (size_t i = 0; i < _traits_directories.size(); ++i)
    {
        for (auto &trait : _traits_directories.at(i).get_traits())
            trait.set_gen_order_trait_directory_id(i);

        _traits_directories[i].set_gen_order(static_cast<int>(i));
    }

    std::sort(_traits_directories.begin(), _traits_directories.end(), [](TraitDirectory &dir1, TraitDirectory &dir2) {
        if (dir1.get_generation_chance() != dir2.get_generation_chance())
        {
            return dir1.get_generation_chance() < dir2.get_generation_chance();
        }
        return dir1.get_traits().size() > dir2.get_traits().size();
    });

    return true;
}

void generator::create_gen_directory(std::string_view dir) const
{
    if (std::filesystem::exists(dir))
        return;

    std::filesystem::create_directory(dir);
}

[[nodiscard]] bool nftgen::generator::add_generated_nft(nftgen::NFT_Metadata &nft_metadata)
{
    size_t traits_hash = nft_metadata.generate_traits_hash();
    if (_nfts_hashes.find(traits_hash) != _nfts_hashes.end())
    {
        return false;
    }

    _nfts_hashes.insert(traits_hash);
    _generated_nfts_metadatas.push_back(nft_metadata);

    for (const auto &trait : nft_metadata.get_traits())
    {
        _nfts_grouped_traits[std::make_pair(trait.get_trait_directory_id(), trait.get_trait_id())]++;
    }

    return true;
}

void nftgen::generator::CheckTraitDirectoryExceotions(NFT_Metadata &nft_metadata, Trait &current_trait, bool &bContinueNextDir, bool &bContinueNextTrait)
{
    std::find_if(_exceptions.traitDirectoriesExceptions.begin(),
                 _exceptions.traitDirectoriesExceptions.end(),
                 [&nft_metadata,
                  &bContinueNextDir,
                  &bContinueNextTrait,
                  &current_trait](const TraitDirectoryException &traitDirException) {
                     std::unique_ptr<nftgen::typedexceptions::IExceptionProcessor> exception_processor =
                         nftgen::typedexceptions::TraitDirectoriesExceptionsFactory::Get(nft_metadata, traitDirException, current_trait);

                     if (exception_processor.get() == nullptr)
                         return false;

                     if (!exception_processor->MeetsGenerationConditions())
                     {
                         bContinueNextDir = exception_processor->ContinueToNextDirectory();
                         bContinueNextTrait = exception_processor->ContinueToNextTrait();

                         return true;
                     }

                     return false;
                 });
}

static bool haveCommonElement(const std::vector<std::string> &arr1, const std::vector<std::string> &arr2)
{
    std::unordered_set<std::string> elements(arr1.begin(), arr1.end());

    for (const std::string &str2 : arr2)
    {
        for (const std::string &str1 : elements)
        {
            if (str1.find(str2) != std::string::npos || str2.find(str1) != std::string::npos)
            {
                return true;
            }
        }
    }
    return false;
}

void nftgen::generator::CheckCrossedExceptions(Trait &currentGeneratedTrait, NFT_Metadata &nftMetadata, bool &continueToNextTrait)
{
    for (const auto &metadataTrait : nftMetadata.get_traits())
    {
        for (const auto &traitException : _exceptions.traitsExceptions)
        {
            if (traitException.exceptionType != ExceptionsTypes::GenerateTraitOnlyWithTraits)
                continue;

            if (metadataTrait.get_directory_name().find(traitException.apply_from_trait_dir) == std::string::npos)
                continue;

            if (metadataTrait.get_filename().find(traitException.apply_from_trait) == std::string::npos)
                continue;

            for (const auto &traitExceptionInternal : _exceptions.traitsExceptions)
            {
                if (traitExceptionInternal.exceptionType != ExceptionsTypes::GenerateTraitOnlyWithTraits)
                    continue;

                if (traitExceptionInternal.apply_from_trait_dir == traitException.apply_from_trait_dir)
                    continue;

                if (traitExceptionInternal.apply_from_trait == traitException.apply_from_trait)
                    continue;

                if (traitException.apply_to_directory != traitExceptionInternal.apply_to_directory)
                    continue;

                if (currentGeneratedTrait.get_directory_name().find(traitException.apply_from_trait_dir) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.get_filename().find(traitException.apply_from_trait) != std::string::npos)
                {
                    continueToNextTrait = true;
                    break;
                }
            }

            if (continueToNextTrait)
                break;
        }

        if (continueToNextTrait)
            break;
    }

    for (const auto &metadataTrait : nftMetadata.get_traits())
    {
        for (const auto &traitException : _exceptions.traitsExceptions)
        {
            if (traitException.exceptionType != ExceptionsTypes::GenerateTraitOnlyWithTraits)
                continue;

            if (metadataTrait.get_directory_name().find(traitException.apply_from_trait_dir) == std::string::npos)
                continue;

            if (metadataTrait.get_filename().find(traitException.apply_from_trait) == std::string::npos)
                continue;

            for (auto &traitExceptionInternal : _exceptions.traitsExceptions)
            {
                if (traitExceptionInternal.exceptionType != ExceptionsTypes::GenerateTraitOnlyWithTraits)
                    continue;

                if (traitExceptionInternal.Compare(traitException))
                    continue;

                if (traitExceptionInternal.apply_to_directory.find(traitException.apply_to_directory) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.get_directory_name().find(traitExceptionInternal.apply_from_trait_dir) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.get_directory_name().find(traitExceptionInternal.apply_from_trait_dir) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.get_filename().find(traitExceptionInternal.apply_from_trait) == std::string::npos)
                    continue;

                const auto &it =
                    std::find_if(nftMetadata.get_traits().begin(),
                                 nftMetadata.get_traits().end(),
                                 [&](Trait &trait) {
                                     if (trait.get_directory_name().find(traitExceptionInternal.apply_from_trait_dir) != std::string::npos)
                                     {
                                         for (const auto &exceptionedTraitInternal : traitExceptionInternal.apply_with_traits.value())
                                         {
                                             if (trait.get_filename().find(exceptionedTraitInternal) != std::string::npos)
                                                 return true;
                                         }
                                     }

                                     return false;
                                 });

                if (it != nftMetadata.get_traits().end())
                    continue;

                if (!haveCommonElement(traitExceptionInternal.apply_with_traits.value(),
                                       traitException.apply_with_traits.value()))
                {
                    continueToNextTrait = true;
                    break;
                }
            }

            if (continueToNextTrait)
                break;
        }

        if (continueToNextTrait)
            break;
    }
}

void nftgen::generator::CheckTraitExceotions(NFT_Metadata &nft_metadata, Trait &current_trait, bool &bContinueNextDir, bool &bContinueNextTrait)
{
    for (auto &metadataTrait : nft_metadata.get_traits())
    {
        for (auto &exception : _exceptions.traitsExceptions)
        {
            if (exception.apply_from_trait == metadataTrait.get_filename() ||
                (metadataTrait.get_directory_name().find(exception.apply_from_trait_dir) != std::string::npos &&
                 metadataTrait.get_filename().find(exception.apply_from_trait) != std::string::npos) ||
                metadataTrait.get_directory_name().find(exception.apply_to_directory) != std::string::npos)
            {
                std::unique_ptr<nftgen::typedexceptions::IExceptionProcessor> exception_processor =
                    nftgen::typedexceptions::TraitExceptionsFactory::Get(nft_metadata, exception, current_trait, _exceptions);

                if (exception_processor.get() == nullptr)
                    continue;

                if (!exception_processor->MeetsGenerationConditions())
                {
                    bContinueNextDir = exception_processor->ContinueToNextDirectory();
                    bContinueNextTrait = exception_processor->ContinueToNextTrait();

                    return;
                }
            }
        }
    }
}

nftgen::generator::generator(std::string trait_directories_root_path) :
    _traits_directories_root_path(trait_directories_root_path)
{
    _generated_nfts_directory = nftgen::settings::get_instance().get_generated_nfts_directory();
    create_gen_directory(_generated_nfts_directory);
}

[[nodiscard]] bool generator::generate(unsigned long nfts_count)
{
    try
    {
        json j;
        std::ifstream nft_metadata_template_file(settings::get_instance().get_template_file());
        nft_metadata_template_file >> j;
        _nft_template_metadata.from_json(j);

        if (!nft_metadata_template_file.is_open())
        {
            std::cerr << "Could not open the templateNFTMetadata file!" << std::endl;
            return false;
        }

        if (!load_directories())
            return false;

        if (_traits_directories.empty())
            return false;

        // -If "Rainbow Mouth" is generated it is generated at the end thus no "Extras" are generated after. - -TODO
        // if money mouth is generated it is generated at the end thus no "Extras" are generated after. - -TODO

        // -If "Hood" is generated you cannot generate "Horns" or "Top Acc".
        _exceptions.traitDirectoriesExceptions.push_back(
            TraitDirectoryException(
                ApplyFromDirectory("Top Acc"),
                ApplyToDirectory("Horns"),
                ExceptionsTypes::NotGenerateDirectoryWithDirectory));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Hood"),
            ApplyFromTraitDirectory("Legacy"),
            NoTraitApply,  // --
            ApplyToDirectory("Horns"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Hood"),
            ApplyFromTraitDirectory("Legacy"),
            NoTraitApply,  // --
            ApplyToDirectory("Top Acc"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        // - If "Beard" is generated it is placed in the 3rd position right before "Eyes" and "Mouths".
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Beard"),
            ApplyFromTraitDirectory("Legacy"),
            RenderAfter("Eyes"),
            ExceptionsTypes::RenderAfter));

        // - If any variation of the "Cap" Trait is generated no "Earings" or "Clothes" with back pieces can be generated.
        _exceptions.traitsExceptions.push_back(TraitException(ApplyFromTrait("Cap"),
                                                              ApplyFromDirectory("Top Acc"),
                                                              NoTraitApply,
                                                              ApplyToDirectory("Earings"),
                                                              ExceptionsTypes::NotGenerateTraitWithDirectory,
                                                              std::nullopt));

        // If any hood is generated, do not generate with backpieces clothes
        _exceptions.traitsExceptions.push_back(TraitException(ApplyFromTrait("Hood"),
                                                              ApplyFromDirectory("Legacy"),
                                                              NoTraitApply,
                                                              ApplyToDirectory("Clothes"),
                                                              ExceptionsTypes::NotGenerateTraitsWithTraits,
                                                              std::vector<std::string>{
                                                                  "Advenaturer Outfit",
                                                                  "Black Angel Wing Robe",
                                                                  "Black Katana Kimono",
                                                                  "Blue Puffy Robe",
                                                                  "Blue Staff Kimono",
                                                                  "Brown Cartographer Shirt",
                                                                  "Brown Katana Kimono",
                                                                  "Dark Blue Cartographer Shirt",
                                                                  "Gold Angel Wing Robe",
                                                                  "Gray Puffy Robe",
                                                                  "Gray Staff Kimono",
                                                                  "Green Archer Outfit",
                                                                  "Green Cartographer Shirt",
                                                                  "Green Katana Kimono",
                                                                  "Green Staff Kimono",
                                                                  "Off-White Angel Wing Robe",
                                                                  "Orange Archer Outfit",
                                                                  "Purple Archer Outfit",
                                                                  "Purple Katana Kimono",
                                                                  "Red Puffy Robe",
                                                                  "Red Over The Shoulder Jacket",
                                                                  "White Staff Kimono",
                                                                  "Yellow Puffy Robe"}));

        _exceptions.traitsExceptions.push_back(TraitException(ApplyFromTrait("Cap"),
                                                              ApplyFromDirectory("Top Acc"),
                                                              NoTraitApply,
                                                              ApplyToDirectory("Clothes"),
                                                              ExceptionsTypes::NotGenerateTraitsWithTraits,
                                                              std::vector<std::string>{
                                                                  "Advenaturer Outfit",
                                                                  "Black Angel Wing Robe",
                                                                  "Black Katana Kimono",
                                                                  "Blue Puffy Robe",
                                                                  "Blue Staff Kimono",
                                                                  "Brown Cartographer Shirt",
                                                                  "Brown Katana Kimono",
                                                                  "Dark Blue Cartographer Shirt",
                                                                  "Gold Angel Wing Robe",
                                                                  "Gray Puffy Robe",
                                                                  "Gray Staff Kimono",
                                                                  "Green Archer Outfit",
                                                                  "Green Cartographer Shirt",
                                                                  "Green Katana Kimono",
                                                                  "Green Staff Kimono",
                                                                  "Off-White Angel Wing Robe",
                                                                  "Orange Archer Outfit",
                                                                  "Purple Archer Outfit",
                                                                  "Purple Katana Kimono",
                                                                  "Red Puffy Robe",
                                                                  "Red Over The Shoulder Jacket",
                                                                  "White Staff Kimono",
                                                                  "Yellow Puffy Robe"}));

        // - "Beanie" trait should only be generated with simple clothing (Stripped Shirt, Tank Top, Hoodie. Tee,)
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Beanie"),
            ApplyFromDirectory("Top Acc"),
            NoTraitApply,  // --
            ApplyToDirectory("Clothes"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Stripped Shirt",
                "Tanktop",
                "Hoodie",
                "Tee"}));

        // - Gold Horns are only generated with Gold Skin
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Gold Horns"),
            ApplyFromDirectory("Horns"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Gold Base"}));

        // - Silver Horns are only generated with Silver Skin
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Silver Horns"),
            ApplyFromDirectory("Horns"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Silver Base"}));

        // - "Flame Horns" should be generated only with darker backgrounds
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Fire Horns"),
            ApplyFromDirectory("Horns"),
            NoTraitApply,  // --
            ApplyToDirectory("Background"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Galaxy Background",
                "Silver Backround",
                "Blue Background",
                "Red Background",
                "Orange Backround"},
            true /*skipReverseCheck*/));

        // - "Anime Eyes" Should be generated with the same color skin - -generate only with
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Blue Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Blue Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Galaxy Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Galaxy Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Gold Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Gold Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Green Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Green",
                "Lime Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Peach Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Peach",
                "Orange Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Pink Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Pink Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Silver Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Silver Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Purple Anime Eyes"),
            ApplyFromDirectory("Eyes"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Galaxy"}));

        // - Any type of "Wood Horns" should not be generated with "Brown Skin"
        // - "Rainbow Mouth" should not be genereted with "Old Time Slot Machine"
        // - "Red Skin" should not be genereted with "Red Kimono"
        // - "Blush Mouth" Should not be genereted with "Silver Skin"
        // - "Anime Eyes" should not be genereted with "Tie" and "Headband" traits

        // - "Golden Glasses" should be generated more with "Scar Eye

        int generated_nfts_count = 0;
        while (generated_nfts_count < nfts_count)
        {
            std::vector<TraitDirectory> has_chance, no_chance;
            for (const auto &trait : _traits_directories)
            {
                if (trait.get_generation_chance() != 1.00)
                {
                    has_chance.push_back(trait);
                }
                else
                {
                    no_chance.push_back(trait);
                }
            }

            std::sort(has_chance.begin(), has_chance.end(), [](const TraitDirectory &a, const TraitDirectory &b) {
                return a.get_generation_chance() < b.get_generation_chance();
            });

            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(no_chance.begin(), no_chance.end(), g);

            _traits_directories.clear();
            _traits_directories.insert(_traits_directories.end(), has_chance.begin(), has_chance.end());
            _traits_directories.insert(_traits_directories.end(), no_chance.begin(), no_chance.end());

            generate_single_nft(generated_nfts_count);
            std::cout << "Generating "
                      << generated_nfts_count
                      << "th NFT"
                      << std::endl;

            system("cls");
        }

        std::vector<std::pair<std::pair<int, int>, size_t>> sorted_traits(_nfts_grouped_traits.begin(), _nfts_grouped_traits.end());

        std::sort(sorted_traits.begin(), sorted_traits.end(), [](const auto &a, const auto &b) {
            return a.first.first < b.first.first;  // Sort by traitFolderID
        });

        std::cout << "Starting NFT plots generation." << std::endl;
        std::string filePath = nftgen::settings::get_instance().get_working_directory() + "\\traits.csv";
        std::ofstream file(filePath);
        if (!file)
        {
            std::cerr << "Error opening file for writing.\n";
            return 1;
        }

        file << "TraitFolderID;TraitID;Count;GenerationChance;\n";

        for (const auto &entry : sorted_traits)
        {
            int trait_directory_id = entry.first.first;  // Folder ID
            int trait_id = entry.first.second;           // Trait ID
            int taritGenerationCount = entry.second;

            auto directory = std::find_if(_traits_directories.begin(), _traits_directories.end(), [&](TraitDirectory &dir) {
                return dir.get_id() == trait_directory_id;
            });

            if (directory == _traits_directories.end())
                continue;

            auto count = 0;
            for (const auto item : sorted_traits)
            {
                if (item.first.first == trait_directory_id)
                    count += item.second;
            }

            const double generation_chance = (static_cast<double>(taritGenerationCount) / count) * 100;

            file
                << directory->get_directory_name()
                << ";"
                << directory->get_traits().at(trait_id).get_filename()
                << ";"
                << taritGenerationCount
                << ";"
                << (generation_chance)
                << "\n";
        }

        file.close();
        std::string command = "python plots.py \"" + filePath + "\" \"" + nftgen::settings::get_instance().get_working_directory() + "\"";

        int result = std::system(command.c_str());
        if (result != 0)
        {
            std::cerr << "Error executing Python script!" << std::endl;
        }
        else
        {
            std::cout << "Python script executed successfully!" << std::endl;
        }

        std::cout << "NFT plots generation finished successfully" << std::endl;

        std::cout << "NFT saving started" << std::endl;
        save_nfts_in_parallel(_generated_nfts_metadatas);
        std::cout << "NFT saving finished successfully" << std::endl;
    }
    catch (const std::exception &exception)
    {
        return false;
    }

    return true;
}

void nftgen::generator::generate_single_nft(int &generated_nft_count)
{
    NFT_Metadata nft_metadata(_nft_template_metadata);

    Trait *current_trait = nullptr;

    auto trait_directory_index = 0;
    bool doNotApplyGenerationChances = false;
    while (trait_directory_index < _traits_directories.size())
    {
        // get_next_trait applies the generation chances, so only exclusion exceptions remain
        auto &&generatedPair = Trait::get_next_trait(trait_directory_index, doNotApplyGenerationChances);
        if (generatedPair.second == SkippedFolder)
        {
            trait_directory_index++;
            continue;
        }
        doNotApplyGenerationChances = false;
        // Something unexpected happened, trying again.
        current_trait = generatedPair.first;
        if (current_trait == nullptr)
            continue;

        bool bContinueNextDir = false;
        bool bContinueNextTrait = false;

        CheckTraitDirectoryExceotions(nft_metadata, *current_trait, bContinueNextDir, bContinueNextTrait);
        if (bContinueNextDir)
        {
            trait_directory_index++;
            continue;
        }

        if (bContinueNextTrait)
        {
            doNotApplyGenerationChances = true;
            continue;
        }
        CheckCrossedExceptions(*current_trait, nft_metadata, bContinueNextTrait);
        if (bContinueNextTrait)
        {
            doNotApplyGenerationChances = true;
            continue;
        }

        CheckTraitExceotions(nft_metadata, *current_trait, bContinueNextDir, bContinueNextTrait);
        if (bContinueNextDir)
        {
            trait_directory_index++;
            continue;
        }

        if (bContinueNextTrait)
        {
            doNotApplyGenerationChances = true;
            continue;
        }

        trait_directory_index++;
        nft_metadata.add_trait(*current_trait);
        continue;
    }

    // if the generated nft hasn't been generated before, we increase the
    // generated nfts count
    if (add_generated_nft(nft_metadata))
    {
        generated_nft_count++;
    }
    else
    {
        std::cout << "Skipping already generated nft with hash:" + std::to_string(nft_metadata.generate_traits_hash()) << std::endl;
    }
}

void nftgen::generator::set_generation_chances(TraitDirectory &trait_directory)
{
    nftgen::calculator calculator;
    calculator.set_equal_generation_chances(trait_directory.get_traits());
}

[[nodiscard]] cv::Mat generator::convert_to_rgba(const cv::Mat &input)
{
    cv::Mat output;

    if (input.channels() == 4)
    {
        output = input.clone();
    }
    else
    {
        cv::cvtColor(input, output, cv::COLOR_BGR2BGRA);
    }

    return output;
}

void generator::alpha_composite(const cv::Mat &base_layer, const cv::Mat &front_layer, cv::Mat &res)
{
    CV_Assert(base_layer.size() == front_layer.size());
    CV_Assert(base_layer.type() == CV_8UC4 && front_layer.type() == CV_8UC4);
    res.create(base_layer.size(), base_layer.type());

    const uchar *base_ptr = base_layer.data;
    const uchar *front_ptr = front_layer.data;
    uchar *result_ptr = res.data;

    int pixels_number = base_layer.rows * base_layer.cols;
    for (int i = 0; i < pixels_number; i++)
    {
        float alpha_B = front_ptr[i * 4 + 3] / 255.0f;
        if (alpha_B == 0 && front_ptr[i * 4 + 2] == 0 && front_ptr[i * 4 + 1] == 0)
            continue;

        for (int c = 0; c < 3; c++)
        {
            result_ptr[i * 4 + c] = static_cast<uchar>(alpha_B * front_ptr[i * 4 + c] + (1 - alpha_B) * base_ptr[i * 4 + c]);
        }

        result_ptr[i * 4 + 3] = 255;
    }
}

void generator::process_nfts(int start_index, int end_index, std::vector<NFT_Metadata> &_generated_nfts)
{
    for (int nft_index = start_index; nft_index < end_index; ++nft_index)
    {
        auto &generated_nft = _generated_nfts[nft_index];
        generated_nft.sort_traits();

        std::vector<TraitException> matchingExceptions;
        std::copy_if(_exceptions.traitsExceptions.begin(), _exceptions.traitsExceptions.end(), std::back_inserter(matchingExceptions), [](const TraitException &exception) {
            return exception.exceptionType == ExceptionsTypes::RenderAfter;
        });

        for (auto &it : matchingExceptions)
        {
            const auto foundTrait = std::find_if(generated_nft.get_traits().begin(), generated_nft.get_traits().end(), [&it](Trait &trait) {
                return trait.get_filename() == it.apply_from_trait;
            });

            if (foundTrait != generated_nft.get_traits().end())
            {
                size_t fromIndex = std::distance(generated_nft.get_traits().begin(), foundTrait);

                size_t toIndex = 2;

                moveElement(generated_nft.get_traits(), fromIndex, toIndex);
            }
        }

        const auto &traits = generated_nft.get_traits();

        cv::Mat base_layer = traits.front().get_matrix();
        base_layer = convert_to_rgba(base_layer);
        cv::Mat res(base_layer.size(), base_layer.type());

        for (int trait_index = 0; trait_index < generated_nft.get_traits().size(); ++trait_index)
        {
            const auto &current_trait = traits[trait_index];

            cv::Mat front_layer = current_trait.get_matrix();
            if (front_layer.empty())
            {
                std::cerr << "Error loading front layer image: " << current_trait.get_filename() << std::endl;
                continue;
            }

            front_layer = convert_to_rgba(front_layer);
            alpha_composite(base_layer, front_layer, res);
            base_layer = res.clone();
        }

        const std::string generated_nft_number = std::to_string(nft_index + 1);
        cv::imwrite(_generated_nfts_directory + "/" + generated_nft_number + ".png", res, {cv::ImwriteFlags::IMWRITE_PNG_COMPRESSION, 1});
        generated_nft.name += " #" + generated_nft_number;
        std::ofstream output_file(_generated_nfts_directory + "/" + generated_nft_number + ".json", std::ios::trunc);
        if (output_file.is_open())
        {
            output_file << generated_nft.to_json().dump(4);
            output_file.close();
            progress_counter += 1;
            if (progress_counter % 10 == 0)
            {
                std::lock_guard<std::mutex> lock(console_mutex);
                std::cout << "Progress: " << progress_counter << " NFTs processed" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to save nft " + generated_nft_number + " metadata" << std::endl;
        }
    }
}

void generator::save_nfts_in_parallel(std::vector<NFT_Metadata> &_generated_nfts)
{
    const int threads_count = 5;
    const int chunk_size = _generated_nfts.size() / threads_count;
    std::vector<std::thread> threads;

    for (int i = 0; i < threads_count; ++i)
    {
        int start_index = i * chunk_size;
        int end_index = (i == threads_count - 1) ? _generated_nfts.size() : (i + 1) * chunk_size;

        threads.push_back(std::thread(&generator::process_nfts, this, start_index, end_index, std::ref(_generated_nfts)));
    }

    for (auto &t : threads)
    {
        t.join();
    }
}

void generator::moveElement(std::vector<Trait> &arr, size_t fromIndex, size_t toIndex)
{
    if (fromIndex == toIndex || fromIndex >= arr.size() || toIndex >= arr.size())
        return;

    Trait temp = std::move(arr[fromIndex]);

    if (fromIndex < toIndex)
    {
        for (size_t i = fromIndex; i < toIndex; ++i)
        {
            arr[i] = std::move(arr[i + 1]);
        }
    }
    else
    {
        for (size_t i = fromIndex; i > toIndex; --i)
        {
            arr[i] = std::move(arr[i - 1]);
        }
    }

    arr[toIndex] = std::move(temp);
}
