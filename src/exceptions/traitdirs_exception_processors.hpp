#pragma once

#include "base_trait_exception_processor.hpp"

namespace nftgen
{
namespace typedexceptions
{

class NotGenerateDirectoryWithDirectoryExceptionProcessor : public BaseTraitDirectoryExceptionProcessor
{
public:
    NotGenerateDirectoryWithDirectoryExceptionProcessor(NFT_Metadata &nftMetadata,
                                                        const Trait &generated_trait,
                                                        const TraitDirectoryException &traitDirectoryException) noexcept
        :
        BaseTraitDirectoryExceptionProcessor(nftMetadata,
                                             generated_trait,
                                             traitDirectoryException)
    {
    }

    virtual ~NotGenerateDirectoryWithDirectoryExceptionProcessor() noexcept = default;

protected:
    [[nodiscard]] virtual inline bool CheckMetadataTrait(const Trait &metadataTrait) override
    {
        const auto &traitDirectoryException = GetTraitDirectoryException();
        const auto &currentGeneratedTrait = GetCurrentGeneratedTrait();
        const auto &generatedTraitDirectory = currentGeneratedTrait.SetDirectoryName();

        if (generatedTraitDirectory.find(traitDirectoryException.targetDirectory) != std::string::npos &&
            metadataTrait.SetDirectoryName().find(traitDirectoryException.sourceDirectory) != std::string::npos)
        {
            SetContinueToNextDirectory(true);
            return false;
        }

        if (generatedTraitDirectory.find(traitDirectoryException.sourceDirectory) != std::string::npos &&
            metadataTrait.SetDirectoryName().find(traitDirectoryException.targetDirectory) != std::string::npos)
        {
            SetContinueToNextDirectory(true);
            return false;
        }

        return true;
    }
};
}  // namespace typedexceptions
}  // namespace nftgen
