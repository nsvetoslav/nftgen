#include "generator.hpp"

#include <filesystem>
#include <format>

#include "generator.hpp"
#include "settings.hpp"
#include "../exceptions/factories.hpp"
#include "../exceptions/exception.hpp"
#include <memory>
#include <random>

#define SourceDirectory(stringVal) stringVal
#define TargetTrait(stringVal) stringVal
#define SourceTrait(stringVal) stringVal
#define SourceTraitDirectory(stringVal) stringVal
#define TargetDirectory(stringVal) stringVal
#define NoTargetTrait ""
#define GenerateOnlyWithTraits(val) val
#define RenderAfter(val) val

using namespace nftgen;

std::vector<TraitDirectory> generator::_traitsDirectories{};

[[nodiscard]] bool generator::LoadDirectories()
{
    std::string rootPath = _traitsDirectoriesRootPath;
    utilities::filemanager fileManager;

    if (!std::filesystem::exists(rootPath) && !std::filesystem::is_directory(rootPath))
    {
        return false;
    }

    // Traversing directories in assets
    std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traitDirectoryPathsPairs;
    fileManager.GetFilePathsInDirectory(rootPath, traitDirectoryPathsPairs);

    for (int traitDirectoryPathIndex = 0; traitDirectoryPathIndex < traitDirectoryPathsPairs.size(); ++traitDirectoryPathIndex)
    {
        auto &traitDirectoryPathPair = traitDirectoryPathsPairs[traitDirectoryPathIndex];
        const auto &traitDirectoryPath = traitDirectoryPathPair.first;
        const auto &traitDirectoryFolderName = traitDirectoryPathPair.second;

        TraitDirectory traitDirectory;
        traitDirectory.SetPath(traitDirectoryPath);
        traitDirectory.SetDirectoryName(traitDirectoryFolderName);
        traitDirectory.SetID(traitDirectoryPathIndex);

        if (traitDirectoryFolderName == "6.Earings")
        {
            traitDirectory.SetGenerationChance(0.06);
        }
        // - Extras should be rarely generated on some of the NFT's (the lowest rate of generated traits
        if (traitDirectoryFolderName == "9.Extras")
        {
            traitDirectory.SetGenerationChance(0.08);
        }
        if (traitDirectoryFolderName == "Legacy")
        {
            traitDirectory.SetGenerationChance(0.03);
        }
        if (traitDirectoryFolderName == "8.Top Acc")
        {
            traitDirectory.SetGenerationChance(0.42);
        }

        std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traitsPaths;
        fileManager.GetFilePathsInDirectory(traitDirectoryPathPair.first, traitsPaths);

        traitDirectory.SetTraits(traitsPaths, traitDirectoryFolderName, traitDirectoryPathIndex, traitDirectoryPathIndex);

        _traitsDirectories.push_back(traitDirectory);
    }

    nftgen::calculator::SetFullGenerationChances(_traitsDirectories);

    for (size_t i = 0; i < _traitsDirectories.size(); ++i)
    {
        for (auto &trait : _traitsDirectories.at(i).GetTraits())
            trait.SetDirectoryGenerationOrderID(i);

        _traitsDirectories[i].SetGenerationOrder(static_cast<int>(i));
    }

    std::sort(_traitsDirectories.begin(), _traitsDirectories.end(), [](TraitDirectory &dir1, TraitDirectory &dir2) {
        if (dir1.GetGenerationChance() != dir2.GetGenerationChance())
        {
            return dir1.GetGenerationChance() < dir2.GetGenerationChance();
        }
        return dir1.GetTraits().size() > dir2.GetTraits().size();
    });

    return true;
}

void generator::CreateGenDirectory(std::string_view dir) const
{
    if (std::filesystem::exists(dir))
        return;

    std::filesystem::create_directory(dir);
}

[[nodiscard]] bool nftgen::generator::AddGeneratedNFT(nftgen::NFT_Metadata &nftMetadata)
{
    size_t traitsHash = nftMetadata.generate_traits_hash();
    if (_nftsHashes.find(traitsHash) != _nftsHashes.end())
    {
        return false;
    }

    _nftsHashes.insert(traitsHash);
    _generatedNFTsMetadatas.push_back(nftMetadata);

    for (const auto &trait : nftMetadata.get_traits())
    {
        _groupedNFTTraits[std::make_pair(trait.GetDirectoryID(), trait.GetTraitID())]++;
    }

    return true;
}

void nftgen::generator::CheckTraitDirectoryExceotions(NFT_Metadata &nftMetadata, Trait &currentTrait, bool &continueNextDir, bool &continueNextTrait)
{
    std::find_if(_exceptions.traitDirectoriesExceptions.begin(),
                 _exceptions.traitDirectoriesExceptions.end(),
                 [&nftMetadata,
                  &continueNextDir,
                  &continueNextTrait,
                  &currentTrait](const TraitDirectoryException &traitDirException) {
                     std::unique_ptr<nftgen::typedexceptions::IExceptionProcessor> exceptionProcessor =
                         nftgen::typedexceptions::TraitDirectoriesExceptionsFactory::Get(nftMetadata, traitDirException, currentTrait);

                     if (exceptionProcessor.get() == nullptr)
                         return false;

                     if (!exceptionProcessor->MeetsGenerationConditions())
                     {
                         continueNextDir = exceptionProcessor->ContinueToNextDirectory();
                         continueNextTrait = exceptionProcessor->ContinueToNextTrait();

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

            if (metadataTrait.SetDirectoryName().find(traitException.sourceDirectory) == std::string::npos)
                continue;

            if (metadataTrait.GetFilename().find(traitException.sourceTrait) == std::string::npos)
                continue;

            for (const auto &traitExceptionInternal : _exceptions.traitsExceptions)
            {
                if (traitExceptionInternal.exceptionType != ExceptionsTypes::GenerateTraitOnlyWithTraits)
                    continue;

                if (traitExceptionInternal.sourceDirectory == traitException.sourceDirectory)
                    continue;

                if (traitExceptionInternal.sourceTrait == traitException.sourceTrait)
                    continue;

                if (traitException.targetDirectory != traitExceptionInternal.targetDirectory)
                    continue;

                if (currentGeneratedTrait.SetDirectoryName().find(traitException.sourceDirectory) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.GetFilename().find(traitException.sourceTrait) != std::string::npos)
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

            if (metadataTrait.SetDirectoryName().find(traitException.sourceDirectory) == std::string::npos)
                continue;

            if (metadataTrait.GetFilename().find(traitException.sourceTrait) == std::string::npos)
                continue;

            for (auto &traitExceptionInternal : _exceptions.traitsExceptions)
            {
                if (traitExceptionInternal.exceptionType != ExceptionsTypes::GenerateTraitOnlyWithTraits)
                    continue;

                if (traitExceptionInternal.Compare(traitException))
                    continue;

                if (traitExceptionInternal.targetDirectory.find(traitException.targetDirectory) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.SetDirectoryName().find(traitExceptionInternal.sourceDirectory) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.SetDirectoryName().find(traitExceptionInternal.sourceDirectory) == std::string::npos)
                    continue;

                if (currentGeneratedTrait.GetFilename().find(traitExceptionInternal.sourceTrait) == std::string::npos)
                    continue;

                const auto &it =
                    std::find_if(nftMetadata.get_traits().begin(),
                                 nftMetadata.get_traits().end(),
                                 [&](Trait &trait) {
                                     if (trait.SetDirectoryName().find(traitExceptionInternal.sourceDirectory) != std::string::npos)
                                     {
                                         for (const auto &exceptionedTraitInternal : traitExceptionInternal.targetTraits.value())
                                         {
                                             if (trait.GetFilename().find(exceptionedTraitInternal) != std::string::npos)
                                                 return true;
                                         }
                                     }

                                     return false;
                                 });

                if (it != nftMetadata.get_traits().end())
                    continue;

                if (!haveCommonElement(traitExceptionInternal.targetTraits.value(),
                                       traitException.targetTraits.value()))
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

void nftgen::generator::CheckTraitExceotions(NFT_Metadata &nftMetadata, Trait &currentTrait, bool &continueNextDir, bool &continueNextTrait)
{
    for (auto &metadataTrait : nftMetadata.get_traits())
    {
        for (auto &exception : _exceptions.traitsExceptions)
        {
            if (exception.sourceTrait == metadataTrait.GetFilename() ||
                (metadataTrait.SetDirectoryName().find(exception.sourceDirectory) != std::string::npos &&
                 metadataTrait.GetFilename().find(exception.sourceTrait) != std::string::npos) ||
                metadataTrait.SetDirectoryName().find(exception.targetDirectory) != std::string::npos)
            {
                std::unique_ptr<nftgen::typedexceptions::IExceptionProcessor> exception_processor =
                    nftgen::typedexceptions::TraitExceptionsFactory::Get(nftMetadata, exception, currentTrait, _exceptions);

                if (exception_processor.get() == nullptr)
                    continue;

                if (!exception_processor->MeetsGenerationConditions())
                {
                    continueNextDir = exception_processor->ContinueToNextDirectory();
                    continueNextTrait = exception_processor->ContinueToNextTrait();

                    return;
                }
            }
        }
    }
}

nftgen::generator::generator(std::string traitDirectoriesRootPath) :
    _traitsDirectoriesRootPath(traitDirectoriesRootPath)
{
    _generatedNFTsDirectory = nftgen::settings::GetInstance().GetGeneratedNFTsDirectory();
    CreateGenDirectory(_generatedNFTsDirectory);
}

[[nodiscard]] bool generator::generate(unsigned long nftsCount)
{
    try
    {
        json j;
        std::ifstream nftsMetadataTemplateFile(settings::GetInstance().GetTemplateFile());
        nftsMetadataTemplateFile >> j;
        _nftTemplateMetadata.from_json(j);

        if (!nftsMetadataTemplateFile.is_open())
        {
            std::cerr << "Could not open the templateNFTMetadata file!" << std::endl;
            return false;
        }

        if (!LoadDirectories())
            return false;

        if (_traitsDirectories.empty())
            return false;

        // -If "Rainbow Mouth" is generated it is generated at the end thus no "Extras" are generated after. - -TODO
        // if money mouth is generated it is generated at the end thus no "Extras" are generated after. - -TODO

        // -If "Hood" is generated you cannot generate "Horns" or "Top Acc".
        _exceptions.traitDirectoriesExceptions.push_back(
            TraitDirectoryException(
                SourceDirectory("Top Acc"),
                TargetDirectory("Horns"),
                ExceptionsTypes::NotGenerateDirectoryWithDirectory));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Hood"),
            SourceTraitDirectory("Legacy"),
            NoTargetTrait,  // --
            TargetDirectory("Horns"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Hood"),
            SourceTraitDirectory("Legacy"),
            NoTargetTrait,  // --
            TargetDirectory("Clothes"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Hood"),
            SourceTraitDirectory("Legacy"),
            NoTargetTrait,  // --
            TargetDirectory("Top Acc"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        // - If "Beard" is generated it is placed in the 3rd position right before "Eyes" and "Mouths".
        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Beard"),
            SourceTraitDirectory("Legacy"),
            RenderAfter("Eyes"),
            ExceptionsTypes::RenderAfter));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Money Mouth"),
            SourceTraitDirectory("Mouth"),
            "",
            ExceptionsTypes::RenderLast));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Rainbow Mouth"),
            SourceTraitDirectory("Mouth"),
            "",
            ExceptionsTypes::RenderLast));

        // - If any variation of the "Cap" Trait is generated no "Earings" or "Clothes" with back pieces can be generated.
        _exceptions.traitsExceptions.push_back(TraitException(SourceTrait("Cap"),
                                                              SourceDirectory("Top Acc"),
                                                              NoTargetTrait,
                                                              TargetDirectory("Earings"),
                                                              ExceptionsTypes::NotGenerateTraitWithDirectory,
                                                              std::nullopt));

        // If any hood is generated, do not generate with backpieces clothes
        _exceptions.traitsExceptions.push_back(TraitException(SourceTrait("Hood"),
                                                              SourceDirectory("Legacy"),
                                                              NoTargetTrait,
                                                              TargetDirectory("Clothes"),
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

        _exceptions.traitsExceptions.push_back(TraitException(SourceTrait("Cap"),
                                                              SourceDirectory("Top Acc"),
                                                              NoTargetTrait,
                                                              TargetDirectory("Clothes"),
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
            SourceTrait("Beanie"),
            SourceDirectory("Top Acc"),
            NoTargetTrait,  // --
            TargetDirectory("Clothes"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Stripped Shirt",
                "Tanktop",
                "Hoodie",
                "Tee"}));

        // - Gold Horns are only generated with Gold Skin
        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Gold Horns"),
            SourceDirectory("Horns"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Gold Base"}));

        // - Silver Horns are only generated with Silver Skin
        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Silver Horns"),
            SourceDirectory("Horns"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Silver Base"}));

        // - "Flame Horns" should be generated only with darker backgrounds
        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Fire Horns"),
            SourceDirectory("Horns"),
            NoTargetTrait,  // --
            TargetDirectory("Background"),
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
            SourceTrait("Blue Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Blue Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Galaxy Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Galaxy Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Gold Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Gold Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Green Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Green",
                "Lime Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Peach Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Peach",
                "Orange Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Pink Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Pink Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Silver Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Silver Base"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Purple Anime Eyes"),
            SourceDirectory("Eyes"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Galaxy"}));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Olive Hood"),
            SourceDirectory("Legacy"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Orange Base"},
            true));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Green Hood"),
            SourceDirectory("Legacy"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Blue Base",
                "Cyan Base"},
            true));

        _exceptions.traitsExceptions.push_back(TraitException(
            SourceTrait("Black Hood"),
            SourceDirectory("Legacy"),
            NoTargetTrait,  // --
            TargetDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Brown Base",
                "White Base",
                "Cow Base"},
            true));

        // - Any type of "Wood Horns" should not be generated with "Brown Skin"
        // - "Rainbow Mouth" should not be genereted with "Old Time Slot Machine"
        // - "Red Skin" should not be genereted with "Red Kimono"
        // - "Blush Mouth" Should not be genereted with "Silver Skin"
        // - "Anime Eyes" should not be genereted with "Tie" and "Headband" traits

        // - "Golden Glasses" should be generated more with "Scar Eye

        int generatedNFTsCount = 0;
        while (generatedNFTsCount < nftsCount)
        {
            std::vector<TraitDirectory> hasGenerationChanceTraitDirectory, NoGenerationChanceTraitDirectory;
            for (const auto &trait : _traitsDirectories)
            {
                if (trait.GetGenerationChance() != 1.00)
                {
                    hasGenerationChanceTraitDirectory.push_back(trait);
                }
                else
                {
                    NoGenerationChanceTraitDirectory.push_back(trait);
                }
            }

            std::sort(hasGenerationChanceTraitDirectory.begin(), hasGenerationChanceTraitDirectory.end(), [](const TraitDirectory &a, const TraitDirectory &b) {
                return a.GetGenerationChance() < b.GetGenerationChance();
            });

            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(NoGenerationChanceTraitDirectory.begin(), NoGenerationChanceTraitDirectory.end(), g);

            _traitsDirectories.clear();
            _traitsDirectories.insert(_traitsDirectories.end(), hasGenerationChanceTraitDirectory.begin(), hasGenerationChanceTraitDirectory.end());
            _traitsDirectories.insert(_traitsDirectories.end(), NoGenerationChanceTraitDirectory.begin(), NoGenerationChanceTraitDirectory.end());

            GenerateSingleNFT(generatedNFTsCount);
            std::cout << "Generating "
                      << generatedNFTsCount
                      << "th NFT"
                      << std::endl;

            system("cls");
        }

        std::vector<std::pair<std::pair<int, int>, size_t>> sortedTraits(_groupedNFTTraits.begin(), _groupedNFTTraits.end());

        std::sort(sortedTraits.begin(), sortedTraits.end(), [](const auto &a, const auto &b) {
            return a.first.first < b.first.first;  // Sort by traitFolderID
        });

        std::cout << "Starting NFT plots generation." << std::endl;
        std::string filePath = nftgen::settings::GetInstance().GetWorkingDirectory() + "\\traits.csv";
        std::ofstream file(filePath);
        if (!file)
        {
            std::cerr << "Error opening file for writing.\n";
            return 1;
        }

        file << "TraitFolderID;TraitID;Count;GenerationChance;\n";

        for (const auto &entry : sortedTraits)
        {
            int traitDirectoryID = entry.first.first;  // Folder ID
            int traitID = entry.first.second;          // Trait ID
            int taritGenerationCount = entry.second;

            auto directory = std::find_if(_traitsDirectories.begin(), _traitsDirectories.end(), [&](TraitDirectory &dir) {
                return dir.GetID() == traitDirectoryID;
            });

            if (directory == _traitsDirectories.end())
                continue;

            auto count = 0;
            for (const auto item : sortedTraits)
            {
                if (item.first.first == traitDirectoryID)
                    count += item.second;
            }

            const double generation_chance = (static_cast<double>(taritGenerationCount) / count) * 100;

            file
                << directory->GetDirectoryName()
                << ";"
                << directory->GetTraits().at(traitID).GetFilename()
                << ";"
                << taritGenerationCount
                << ";"
                << (generation_chance)
                << "\n";
        }

        file.close();
        std::string command = "python plots.py \"" + filePath + "\" \"" + nftgen::settings::GetInstance().GetWorkingDirectory() + "\"";

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
        SaveNTFSInParrallel(_generatedNFTsMetadatas);
        std::cout << "NFT saving finished successfully" << std::endl;
    }
    catch (const std::exception &exception)
    {
        return false;
    }

    return true;
}

void nftgen::generator::GenerateSingleNFT(int &generatedNFTsCount)
{
    NFT_Metadata nftMetadata(_nftTemplateMetadata);

    Trait *currentTrait = nullptr;

    auto currentTraitDirectoryIndex = 0;
    bool doNotApplyGenerationChances = false;
    while (currentTraitDirectoryIndex < _traitsDirectories.size())
    {
        // get_next_trait applies the generation chances, so only exclusion exceptions remain
        const auto &generatedPair = Trait::GetNextTrait(currentTraitDirectoryIndex, doNotApplyGenerationChances);
        if (generatedPair.second == SkippedFolder)
        {
            currentTraitDirectoryIndex++;
            continue;
        }

        doNotApplyGenerationChances = false;
        // Something unexpected happened, trying again.
        currentTrait = generatedPair.first;
        if (currentTrait == nullptr)
            continue;

        bool bContinueNextDir = false;
        bool bContinueNextTrait = false;

        CheckTraitDirectoryExceotions(nftMetadata, *currentTrait, bContinueNextDir, bContinueNextTrait);
        if (bContinueNextDir)
        {
            currentTraitDirectoryIndex++;
            continue;
        }

        if (bContinueNextTrait)
        {
            doNotApplyGenerationChances = true;
            continue;
        }

        CheckCrossedExceptions(*currentTrait, nftMetadata, bContinueNextTrait);
        if (bContinueNextTrait)
        {
            doNotApplyGenerationChances = true;
            continue;
        }

        CheckTraitExceotions(nftMetadata, *currentTrait, bContinueNextDir, bContinueNextTrait);
        if (bContinueNextDir)
        {
            currentTraitDirectoryIndex++;
            continue;
        }

        if (bContinueNextTrait)
        {
            doNotApplyGenerationChances = true;
            continue;
        }

        currentTraitDirectoryIndex++;
        nftMetadata.add_trait(*currentTrait);
        continue;
    }

    // if the generated nft hasn't been generated before, we increase the
    // generated nfts count
    if (AddGeneratedNFT(nftMetadata))
    {
        generatedNFTsCount++;
    }
    else
    {
        std::cout << "Skipping already generated nft with hash:" + std::to_string(nftMetadata.generate_traits_hash()) << std::endl;
    }
}

void nftgen::generator::SetGenerationChances(TraitDirectory &traitDirectory)
{
    nftgen::calculator calculator;
    calculator.SetEqualGenerationChances(traitDirectory.GetTraits());
}

[[nodiscard]] cv::Mat generator::ConvertToRgba(const cv::Mat &input)
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

void generator::AlphaComposite(const cv::Mat &baseLayer, const cv::Mat &frontLayer, cv::Mat &res)
{
    CV_Assert(baseLayer.size() == frontLayer.size());
    CV_Assert(baseLayer.type() == CV_8UC4 && frontLayer.type() == CV_8UC4);
    res.create(baseLayer.size(), baseLayer.type());

    const uchar *basePtr = baseLayer.data;
    const uchar *frontPtr = frontLayer.data;
    uchar *resultPtr = res.data;

    int pixelsNumber = baseLayer.rows * baseLayer.cols;
    for (int i = 0; i < pixelsNumber; i++)
    {
        float alpha_B = frontPtr[i * 4 + 3] / 255.0f;
        if (alpha_B == 0 && frontPtr[i * 4 + 2] == 0 && frontPtr[i * 4 + 1] == 0)
            continue;

        for (int c = 0; c < 3; c++)
        {
            resultPtr[i * 4 + c] = static_cast<uchar>(alpha_B * frontPtr[i * 4 + c] + (1 - alpha_B) * basePtr[i * 4 + c]);
        }

        resultPtr[i * 4 + 3] = 255;
    }
}

void generator::ProcessNFTs(int startIndex, int endIndex, std::vector<NFT_Metadata> &generatedNFTs)
{
    for (int currentNFTIndex = startIndex; currentNFTIndex < endIndex; ++currentNFTIndex)
    {
        auto &generatedNFT = generatedNFTs[currentNFTIndex];
        generatedNFT.sort_traits();

        std::vector<TraitException> matchingExceptions;
        std::copy_if(_exceptions.traitsExceptions.begin(), _exceptions.traitsExceptions.end(), std::back_inserter(matchingExceptions), [](const TraitException &exception) {
            return exception.exceptionType == ExceptionsTypes::RenderAfter;
        });

        for (auto &it : matchingExceptions)
        {
            const auto foundTrait = std::find_if(generatedNFT.get_traits().begin(), generatedNFT.get_traits().end(), [&it](Trait &trait) {
                return trait.GetFilename() == it.sourceTrait;
            });

            if (foundTrait != generatedNFT.get_traits().end())
            {
                size_t fromIndex = std::distance(generatedNFT.get_traits().begin(), foundTrait);

                size_t toIndex = 2;

                MoveElement(generatedNFT.get_traits(), fromIndex, toIndex);
            }
        }

        matchingExceptions.clear();
        std::copy_if(_exceptions.traitsExceptions.begin(), _exceptions.traitsExceptions.end(), std::back_inserter(matchingExceptions), [](const TraitException &exception) {
            return exception.exceptionType == ExceptionsTypes::RenderLast;
        });

        for (auto &it : matchingExceptions)
        {
            const auto foundTrait = std::find_if(generatedNFT.get_traits().begin(), generatedNFT.get_traits().end(), [&it](Trait &trait) {
                return trait.GetFilename() == it.sourceTrait;
            });

            if (foundTrait != generatedNFT.get_traits().end())
                std::iter_swap(foundTrait, generatedNFT.get_traits().end() - 1);
        }

        const auto &traits = generatedNFT.get_traits();

        cv::Mat baseLayer = traits.front().GetMatrix();
        baseLayer = ConvertToRgba(baseLayer);
        cv::Mat res(baseLayer.size(), baseLayer.type());

        for (int traitIndex = 0; traitIndex < generatedNFT.get_traits().size(); ++traitIndex)
        {
            const auto &current_trait = traits[traitIndex];

            cv::Mat frontLayer = current_trait.GetMatrix();
            if (frontLayer.empty())
            {
                std::cerr << "Error loading front layer image: " << current_trait.GetFilename() << std::endl;
                continue;
            }

            frontLayer = ConvertToRgba(frontLayer);
            AlphaComposite(baseLayer, frontLayer, res);
            baseLayer = res.clone();
        }

        const std::string generatedNFTNumber = std::to_string(currentNFTIndex + 1);
        cv::imwrite(_generatedNFTsDirectory + "/" + generatedNFTNumber + ".png", res, {cv::ImwriteFlags::IMWRITE_PNG_COMPRESSION, 1});
        generatedNFT.name += " #" + generatedNFTNumber;
        std::ofstream outputFile(_generatedNFTsDirectory + "/" + generatedNFTNumber + ".json", std::ios::trunc);
        if (outputFile.is_open())
        {
            outputFile << generatedNFT.to_json().dump(4);
            outputFile.close();
            _progressCounter += 1;
            if (_progressCounter % 10 == 0)
            {
                std::lock_guard<std::mutex> lock(consoleMutex);
                std::cout << "Progress: " << _progressCounter << " NFTs processed" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to save nft " + generatedNFTNumber + " metadata" << std::endl;
        }
    }
}

void generator::SaveNTFSInParrallel(std::vector<NFT_Metadata> &generatedNFTs)
{
    const int threadsCount = 5;
    const int chunkSize = generatedNFTs.size() / threadsCount;
    std::vector<std::thread> threads;

    for (int i = 0; i < threadsCount; ++i)
    {
        int startIndex = i * chunkSize;
        int endIndex = (i == threadsCount - 1) ? generatedNFTs.size() : (i + 1) * chunkSize;

        threads.push_back(std::thread(&generator::ProcessNFTs, this, startIndex, endIndex, std::ref(generatedNFTs)));
    }

    for (auto &t : threads)
    {
        t.join();
    }
}

void generator::MoveElement(std::vector<Trait> &arr, size_t fromIndex, size_t toIndex)
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
