#pragma once

#include "base_trait_exception_processor.hpp"

namespace nftgen
{
namespace typedexceptions
{
class NotGenerateTraitWithDirectoryExceptionProcessor : public BaseTraitExceptionProcessor
{
public:
    NotGenerateTraitWithDirectoryExceptionProcessor(NFT_Metadata &nftMetadata,
                                                    const Trait &generated_trait,
                                                    const TraitException &traitException) noexcept
        :
        BaseTraitExceptionProcessor(nftMetadata,
                                    generated_trait,
                                    traitException)
    {
    }

    virtual ~NotGenerateTraitWithDirectoryExceptionProcessor() noexcept = default;

protected:
    [[nodiscard]] virtual inline bool CheckMetadataTrait(const Trait &metadataTrait) override
    {
        const auto &traitException = GetTraitException();
        const auto &currentGeneratedTrait = GetCurrentGeneratedTrait();

        const auto &generatedTraitDirectory = currentGeneratedTrait.SetDirectoryName();

        if (generatedTraitDirectory.find(traitException.targetDirectory) != std::string::npos &&
            metadataTrait.SetDirectoryName().find(traitException.sourceDirectory) != std::string::npos &&
            metadataTrait.GetFilename().find(traitException.sourceTrait) != std::string::npos)
        {
            SetContinueToNextDirectory(true);
            return false;
        }

        if (metadataTrait.SetDirectoryName().find(traitException.targetDirectory) != std::string::npos)
        {
            if (currentGeneratedTrait.SetDirectoryName().find(traitException.sourceDirectory) != std::string::npos)
            {
                if (currentGeneratedTrait.GetFilename().find(traitException.sourceTrait) != std::string::npos)
                {
                    SetContinueToNextDirectory(true);
                    return false;
                }
            }
        }

        return true;
    }
};

class GenerateTraitOnlyWithTraitsExceptionProcessor : public BaseTraitExceptionProcessor
{
public:
    GenerateTraitOnlyWithTraitsExceptionProcessor(NFT_Metadata &nftMetadata,
                                                  const Trait &generatedTrait,
                                                  const TraitException &traitException,
                                                  Exceptions &allExceptions) noexcept
        :
        BaseTraitExceptionProcessor(nftMetadata,
                                    generatedTrait,
                                    traitException),
        m_allExceptions(allExceptions)
    {
    }

    virtual ~GenerateTraitOnlyWithTraitsExceptionProcessor() noexcept = default;

private:
    [[nodiscard]] bool ValidateException(const Trait &metadataTrait)
    {
        const auto &traitException = GetTraitException();
        const auto &currentGeneratedTrait = GetCurrentGeneratedTrait();
        const auto &generatedTraitDirectory = currentGeneratedTrait.SetDirectoryName();

        const bool &&isMetadataTraitDirectorySameAsApplyFromTraitDir =
            metadataTrait.SetDirectoryName().find(traitException.sourceDirectory) != std::string::npos;

        if (!isMetadataTraitDirectorySameAsApplyFromTraitDir)
            return true;

        if (metadataTrait.GetFilename().find(traitException.sourceTrait) == std::string::npos)
            return true;

        if (currentGeneratedTrait.SetDirectoryName().find(traitException.targetDirectory) == std::string::npos)
            return true;

        const auto it = std::find_if(traitException.targetTraits->begin(),
                                     traitException.targetTraits->end(),
                                     [&currentGeneratedTrait](const std::string exceptedTraitName) {
                                         if (currentGeneratedTrait.GetFilename().find(exceptedTraitName) != std::string::npos)
                                         {
                                             return true;
                                         }

                                         return false;
                                     });

        if (it == traitException.targetTraits->end())
        {
            SetContinueToNextTrait(true);
            return false;
        }

        return true;
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

    [[nodiscard]] bool ValidateReversedException(const Trait &metadataTrait)
    {
        const auto &traitException = GetTraitException();
        const auto &currentGeneratedTrait = GetCurrentGeneratedTrait();
        const auto &generatedTraitDirectory = currentGeneratedTrait.SetDirectoryName();

        const bool &&isMetadataTraitDirectorySameAsApplyFromTraitDir =
            metadataTrait.SetDirectoryName().find(traitException.targetDirectory) != std::string::npos;

        if (!isMetadataTraitDirectorySameAsApplyFromTraitDir)
            return true;

        const auto it = std::find_if(traitException.targetTraits->begin(),
                                     traitException.targetTraits->end(),
                                     [&metadataTrait](const std::string exceptedTraitName) {
                                         if (metadataTrait.GetFilename().find(exceptedTraitName) != std::string::npos)
                                         {
                                             return true;
                                         }

                                         return false;
                                     });

        if (it == traitException.targetTraits->end())
        {
            if (currentGeneratedTrait.GetFilename().find(traitException.sourceTrait) != std::string::npos)
            {
                if (metadataTrait.GetFilename().find(currentGeneratedTrait.GetFilename()) == std::string::npos)
                {
                    SetContinueToNextTrait(true);
                    return false;
                }
            }

            return true;
        }

        if (currentGeneratedTrait.SetDirectoryName().find(traitException.sourceDirectory) == std::string::npos)
            return true;

        if (currentGeneratedTrait.GetFilename().find(traitException.sourceTrait) == std::string::npos)
        {
            for (auto &exception : m_allExceptions.traitsExceptions)
            {
                if (exception.Compare(traitException))
                    continue;

                if (exception.exceptionType != exception.exceptionType)
                    continue;

                if (exception.targetDirectory != traitException.targetDirectory)
                    continue;

                const auto &it = std::find_if(exception.targetTraits->begin(),
                                              exception.targetTraits->end(),
                                              [&](std::string targetTrait) {
                                                  return metadataTrait.GetFilename().find(targetTrait) != std::string::npos;
                                              });

                if (it != exception.targetTraits->end() && currentGeneratedTrait.GetFilename().find(*it) != std::string::npos)
                {
                    return true;
                }

                if (haveCommonElement(traitException.targetTraits.value(), exception.targetTraits.value()))
                    return true;
            }

            SetContinueToNextTrait(true);
            return false;
        }

        return true;
    }

    [[nodiscard]] bool ValidateCrossExceptions(const Trait &metadata_trait)
    {
        return true;
    }

protected:
    [[nodiscard]] virtual inline bool CheckMetadataTrait(const Trait &metadata_trait) override
    {
        if (!ValidateException(metadata_trait))
            return false;

        if (!GetTraitException().skipReverseCheck && !ValidateReversedException(metadata_trait))
            return false;

        return true;
    }

    Exceptions &m_allExceptions;
};

}  // namespace typedexceptions
}  // namespace nftgen
