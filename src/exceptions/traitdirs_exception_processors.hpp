#pragma once

#include "base_trait_exception_processor.hpp"

namespace nftgen
{
namespace typedexceptions
{

class NotGenerateDirectoryWithDirectoryExceptionProcessor : public BaseTraitDirectoryExceptionProcessor
{
public:
    NotGenerateDirectoryWithDirectoryExceptionProcessor(const NFT_Metadata &nftMetadata,
                                                        const Trait &generated_trait,
                                                        const TraitDirectoryException &trait_directory_exception) noexcept
        :
        BaseTraitDirectoryExceptionProcessor(nftMetadata,
                                             generated_trait,
                                             trait_directory_exception)
    {
    }

    virtual ~NotGenerateDirectoryWithDirectoryExceptionProcessor() noexcept = default;

protected:
    [[nodiscard]] virtual inline bool CheckMetadataTrait(const Trait &metadata_trait) override
    {
        const auto &trait_exception = GetTraitDirectoryException();
        const auto &current_generated_trait = GetCurrentGeneratedTrait();
        const auto &generated_trait_dir_name = current_generated_trait.get_directory_name();

        if (generated_trait_dir_name.find(trait_exception.apply_to_directory) != std::string::npos &&
            metadata_trait.get_directory_name().find(trait_exception.apply_from_directory) != std::string::npos)
        {
            SetContinueToNextDirectory(true);
            return false;
        }

        if (generated_trait_dir_name.find(trait_exception.apply_from_directory) != std::string::npos &&
            metadata_trait.get_directory_name().find(trait_exception.apply_to_directory) != std::string::npos)
        {
            SetContinueToNextDirectory(true);
            return false;
        }

        return true;
    }
};
}  // namespace typedexceptions
}  // namespace nftgen
