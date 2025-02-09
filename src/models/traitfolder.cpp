#include "traitfolder.hpp"
#include "../workflow/calculator.hpp"
#include "../workflow/stringutilities.hpp"

namespace nftgen
{

void TraitDirectory::SetPath(std::string path)
{
    _path = path;
}

void TraitDirectory::SetDirectoryName(std::string directoryName)
{
    _directoryName = directoryName;
}

void TraitDirectory::SetTraits(std::vector<std::pair<std::string /*path*/, std::string> /*file_name*/> traits,
                               std::string directoryName,
                               int &directoryGenerationID,
                               int &directoryID)
{
    for (int traitID = 0; traitID < traits.size(); ++traitID)
    {
        const auto &traitPath = traits[traitID];

        cv::Mat &&imageMatrix = cv::imread(traitPath.first, cv::IMREAD_UNCHANGED);
        Trait trait;
        trait.SetPath(traitPath.first);
        trait.SetFilename(nftgen::utilities::stringutilities::ClearPostfix(traitPath.second));
        trait.GetDirectoryName(nftgen::utilities::stringutilities::ClearPrefix(directoryName));
        trait.SetDirectoryGenerationOrderID(directoryGenerationID);
        trait.SetTraitID(traitID);
        trait.SetDirectoryID(directoryID);
        trait.SetMatrix(imageMatrix);
        //-Ranbow Mouth Should be generated less
        //-"Portal Eyes" Should be generated less
        //-"7 Eyes" Should be generated less
        //-"Chip Eyes" Should be generated less
        //-"Flame Horns" should be generated only with darker backgrounds
        //-"Card suit eyes" (spades, hearts, diamonds, spades) should be genereted less

        if (trait.GetFilename().find("Rainbow Mouth") != std::string::npos)
            trait.SetGenerationChance(0.009);

        else if (trait.GetFilename().find("Portal Eyes") != std::string::npos)
            trait.SetGenerationChance(0.008);

        else if (trait.GetFilename().find("77 Eyes") != std::string::npos)
            trait.SetGenerationChance(0.005);

        else if (trait.GetFilename().find("Chip Eyes") != std::string::npos)
            trait.SetGenerationChance(0.008);

        else if (trait.GetFilename().find("Flame Horns") != std::string::npos)
            trait.SetGenerationChance(0.01);

        else if (trait.GetFilename().find("Spade Eyes") != std::string::npos)
            trait.SetGenerationChance(0.008);

        else if (trait.GetFilename().find("Hearts Eyes") != std::string::npos)
            trait.SetGenerationChance(0.007);

        else if (trait.GetFilename().find("Heart Eyes V1") != std::string::npos)
            trait.SetGenerationChance(0.007);

        else if (trait.GetFilename().find("Heart Eyes V2") != std::string::npos)
            trait.SetGenerationChance(0.007);

        else if (trait.GetFilename().find("Diamond Eyes") != std::string::npos)
            trait.SetGenerationChance(0.007);

        else if (trait.GetFilename().find("Spade Eyes") != std::string::npos)
            trait.SetGenerationChance(0.008);

        else if (trait.GetFilename().find("Exhale Mouth") != std::string::npos)
            trait.SetGenerationChance(0.005);

        _traits.push_back(trait);
    }

    nftgen::calculator::SetEqualGenerationChances(_traits);
}

[[nodiscard]] const int &TraitDirectory::GetID() const
{
    return _id;
}

void TraitDirectory::SetID(int id)
{
    _id = id;
}

[[nodiscard]] std::vector<nftgen::Trait> &TraitDirectory::GetTraits()
{
    return _traits;
}

[[nodiscard]] const double TraitDirectory::GetGenerationChance() const
{
    return _generationChance;
}

void TraitDirectory::SetGenerationChance(double generationChance)
{
    _generationChance = generationChance;
}

int TraitDirectory::GetGenerationOrder() const
{
    return _generationOrder;
}

void TraitDirectory::SetGenerationOrder(int generationOrder)
{
    _generationOrder = generationOrder;
}

[[nodiscard]] std::string_view TraitDirectory::GetPath()
{
    return _path;
}

[[nodiscard]] std::string TraitDirectory::GetDirectoryName()
{
    return _directoryName;
}

}  // namespace nftgen
