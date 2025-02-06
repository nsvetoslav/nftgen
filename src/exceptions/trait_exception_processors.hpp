#pragma once

#include "base_trait_exception_processor.hpp"

namespace nftgen
{
namespace typedexceptions
{
class NotGenerateTraitWithDirectoryExceptionProcessor : public BaseTraitExceptionProcessor
{
public:
    NotGenerateTraitWithDirectoryExceptionProcessor(const NFT_Metadata &nftMetadata,
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
    GenerateTraitOnlyWithTraitsExceptionProcessor(const NFT_Metadata &nftMetadata,
                                                  const Trait &generated_trait,
                                                  const TraitException &trait_exception) noexcept
        :
        BaseTraitExceptionProcessor(nftMetadata,
                                    generated_trait,
                                    trait_exception)
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
            return true;

        if (current_generated_trait.get_directory_name().find(trait_exception.apply_from_trait_dir) == std::string::npos)
            return true;

        if (current_generated_trait.get_filename().find(trait_exception.apply_from_trait) == std::string::npos)
        {
            SetContinueToNextTrait(true);
            return false;
        }

        return true;
    }

    [[nodiscard]] bool ValidateReversedException(const Trait &metadata_trait)
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

protected:
    [[nodiscard]] virtual inline bool CheckMetadataTrait(const Trait &metadata_trait) override
    {
        if (!ValidateException(metadata_trait))
            return false;

        if (!ValidateReversedException(metadata_trait))
            return false;

        return true;
    }
};

}  // namespace typedexceptions
}  // namespace nftgen
