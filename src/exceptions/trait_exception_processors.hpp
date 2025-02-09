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
                                                    const TraitException &trait_exception) noexcept
        :
        BaseTraitExceptionProcessor(nftMetadata,
                                    generated_trait,
                                    trait_exception)
    {
    }

    virtual ~NotGenerateTraitWithDirectoryExceptionProcessor() noexcept = default;

protected:
    [[nodiscard]] virtual inline bool CheckMetadataTrait(const Trait &metadata_trait) override
    {
        const auto &trait_exception = GetTraitException();
        const auto &current_generated_trait = GetCurrentGeneratedTrait();

        const auto &generated_trait_dir_name = current_generated_trait.get_directory_name();

        if (generated_trait_dir_name.find(trait_exception.apply_to_directory) != std::string::npos &&
            metadata_trait.get_directory_name().find(trait_exception.apply_from_trait_dir) != std::string::npos &&
            metadata_trait.get_filename().find(trait_exception.apply_from_trait) != std::string::npos)
        {
            SetContinueToNextDirectory(true);
            return false;
        }

        if (metadata_trait.get_directory_name().find(trait_exception.apply_to_directory) != std::string::npos)
        {
            if (current_generated_trait.get_directory_name().find(trait_exception.apply_from_trait_dir) != std::string::npos)
            {
                if (current_generated_trait.get_filename().find(trait_exception.apply_from_trait) != std::string::npos)
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
                                                  const Trait &generated_trait,
                                                  const TraitException &trait_exception,
                                                  Exceptions &allExceptions) noexcept
        :
        BaseTraitExceptionProcessor(nftMetadata,
                                    generated_trait,
                                    trait_exception),
        m_allExceptions(allExceptions)
    {
    }

    virtual ~GenerateTraitOnlyWithTraitsExceptionProcessor() noexcept = default;

private:
    [[nodiscard]] bool ValidateException(const Trait &metadata_trait)
    {
        const auto &trait_exception = GetTraitException();
        const auto &current_generated_trait = GetCurrentGeneratedTrait();
        const auto &generated_trait_dir_name = current_generated_trait.get_directory_name();

        const bool &&isMetadataTraitDirectorySameAsApplyFromTraitDir =
            metadata_trait.get_directory_name().find(trait_exception.apply_from_trait_dir) != std::string::npos;

        if (!isMetadataTraitDirectorySameAsApplyFromTraitDir)
            return true;

        if (metadata_trait.get_filename().find(trait_exception.apply_from_trait) == std::string::npos)
            return true;

        if (current_generated_trait.get_directory_name().find(trait_exception.apply_to_directory) == std::string::npos)
            return true;

        const auto it = std::find_if(trait_exception.apply_with_traits->begin(),
                                     trait_exception.apply_with_traits->end(),
                                     [&current_generated_trait](const std::string exceptedTraitName) {
                                         if (current_generated_trait.get_filename().find(exceptedTraitName) != std::string::npos)
                                         {
                                             return true;
                                         }

                                         return false;
                                     });

        if (it == trait_exception.apply_with_traits->end())
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
                    return true;  // Found a partial match
                }
            }
        }

        return false;  // No common elements found
    }

    [[nodiscard]] bool ValidateReversedException(const Trait &metadata_trait)
    {
        const auto &trait_exception = GetTraitException();
        const auto &current_generated_trait = GetCurrentGeneratedTrait();
        const auto &generated_trait_dir_name = current_generated_trait.get_directory_name();

        const bool &&isMetadataTraitDirectorySameAsApplyFromTraitDir =
            metadata_trait.get_directory_name().find(trait_exception.apply_to_directory) != std::string::npos;

        if (!isMetadataTraitDirectorySameAsApplyFromTraitDir)
            return true;

        const auto it = std::find_if(trait_exception.apply_with_traits->begin(),
                                     trait_exception.apply_with_traits->end(),
                                     [&metadata_trait](const std::string exceptedTraitName) {
                                         if (metadata_trait.get_filename().find(exceptedTraitName) != std::string::npos)
                                         {
                                             return true;
                                         }

                                         return false;
                                     });

        if (it == trait_exception.apply_with_traits->end())
        {
            if (current_generated_trait.get_filename().find(trait_exception.apply_from_trait) != std::string::npos)
            {
                if (metadata_trait.get_filename().find(current_generated_trait.get_filename()) == std::string::npos)
                {
                    SetContinueToNextTrait(true);
                    return false;
                }
            }

            return true;
        }

        if (current_generated_trait.get_directory_name().find(trait_exception.apply_from_trait_dir) == std::string::npos)
            return true;

        if (current_generated_trait.get_filename().find(trait_exception.apply_from_trait) == std::string::npos)
        {
            for (auto &exception : m_allExceptions.traitsExceptions)
            {
                if (exception.Compare(trait_exception))
                    continue;

                if (exception.exceptionType != exception.exceptionType)
                    continue;

                if (exception.apply_to_directory != trait_exception.apply_to_directory)
                    continue;

                const auto &it = std::find_if(exception.apply_with_traits->begin(),
                                              exception.apply_with_traits->end(),
                                              [&](std::string targetTrait) {
                                                  return metadata_trait.get_filename().find(targetTrait) != std::string::npos;
                                              });

                if (it != exception.apply_with_traits->end() && current_generated_trait.get_filename().find(*it) != std::string::npos)
                {
                    return true;
                }

                if (haveCommonElement(trait_exception.apply_with_traits.value(), exception.apply_with_traits.value()))
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

        if (!GetTraitException().bSkipReverseCheck && !ValidateReversedException(metadata_trait))
            return false;

        return true;
    }

    Exceptions &m_allExceptions;
};

}  // namespace typedexceptions
}  // namespace nftgen
