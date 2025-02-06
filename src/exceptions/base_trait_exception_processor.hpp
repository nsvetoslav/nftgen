#pragma once

#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include "exception.hpp"
#include "../models/exceptions.hpp"
#include "../models/trait.hpp"
#include "../models/nft_template.hpp"

namespace nftgen
{
namespace typedexceptions
{

class BaseTraitExceptionProcessor : public IExceptionProcessor
{
public:
    BaseTraitExceptionProcessor(const NFT_Metadata &nftMetadata, const Trait &generated_trait, const TraitException &trait_exception) noexcept
        :
        _generated_trait(generated_trait), _trait_exception(trait_exception), _nftMetadata(nftMetadata)
    {
    }

    virtual ~BaseTraitExceptionProcessor() noexcept = default;

public:
    [[nodiscard]] inline bool MeetsGenerationConditions() override
    {
        return MeetsGenerationConditions(_nftMetadata, _generated_trait, _trait_exception);
    }

    inline bool ContinueToNextDirectory() const override
    {
        return continue_to_next_directory;
    }

    inline bool ContinueToNextTrait() const override
    {
        return continue_to_next_trait;
    }

private:
    [[nodiscard]] virtual inline bool MeetsGenerationConditions(const NFT_Metadata &nftMetadata, const Trait &_trait, const TraitException context)
    {
        for (const Trait &metadata_trait : nftMetadata.get_traits())
        {
            if (!CheckMetadataTrait(metadata_trait))
                return false;
        }

        return true;
    }

protected:
    [[nodiscard]] virtual bool CheckMetadataTrait(const Trait &metadata_trait) = 0;

protected:
    [[nodiscard]] inline const NFT_Metadata &GetNftMetadata() const
    {
        return _nftMetadata;
    }

    [[nodiscard]] inline const Trait &GetCurrentGeneratedTrait() const
    {
        return _generated_trait;
    }

    [[nodiscard]] inline const TraitException &GetTraitException() const
    {
        return _trait_exception;
    }

    inline void SetContinueToNextTrait(bool continue_to_next_trait)
    {
        this->continue_to_next_trait = continue_to_next_trait;
    }

    inline void SetContinueToNextDirectory(bool continue_to_next_directory)
    {
        this->continue_to_next_directory = continue_to_next_directory;
    }

private:
    const NFT_Metadata &_nftMetadata;
    const Trait &_generated_trait;
    const TraitException _trait_exception;

    bool continue_to_next_trait{};
    bool continue_to_next_directory{};
};

class BaseTraitDirectoryExceptionProcessor : public IExceptionProcessor
{
public:
    BaseTraitDirectoryExceptionProcessor(const NFT_Metadata &nftMetadata,
                                         const Trait &generated_trait,
                                         const TraitDirectoryException &trait_directory_exception) noexcept
        :
        _generated_trait(generated_trait), _trait_directory_exception(trait_directory_exception), _nftMetadata(nftMetadata)
    {
    }

    virtual ~BaseTraitDirectoryExceptionProcessor() noexcept = default;

public:
    [[nodiscard]] inline bool MeetsGenerationConditions() override
    {
        return MeetsGenerationConditions(_nftMetadata, _generated_trait, _trait_directory_exception);
    }

    inline bool ContinueToNextDirectory() const override
    {
        return continue_to_next_directory;
    }

    inline bool ContinueToNextTrait() const override
    {
        return continue_to_next_trait;
    }

private:
    [[nodiscard]] virtual inline bool MeetsGenerationConditions(const NFT_Metadata &nftMetadata,
                                                                const Trait &_trait,
                                                                const TraitDirectoryException context)
    {
        for (const Trait &metadata_trait : nftMetadata.get_traits())
        {
            if (!CheckMetadataTrait(metadata_trait))
                return false;
        }

        return true;
    }

protected:
    [[nodiscard]] virtual bool CheckMetadataTrait(const Trait &metadata_trait) = 0;

protected:
    [[nodiscard]] inline const NFT_Metadata &GetNftMetadata() const
    {
        return _nftMetadata;
    }

    [[nodiscard]] inline const Trait &GetCurrentGeneratedTrait() const
    {
        return _generated_trait;
    }

    [[nodiscard]] inline const TraitDirectoryException &GetTraitDirectoryException() const
    {
        return _trait_directory_exception;
    }

    inline void SetContinueToNextTrait(bool continue_to_next_trait)
    {
        this->continue_to_next_trait = continue_to_next_trait;
    }

    inline void SetContinueToNextDirectory(bool continue_to_next_directory)
    {
        this->continue_to_next_directory = continue_to_next_directory;
    }

private:
    const NFT_Metadata &_nftMetadata;
    const Trait &_generated_trait;
    const TraitDirectoryException _trait_directory_exception;

    bool continue_to_next_trait{};
    bool continue_to_next_directory{};
};

}  // namespace typedexceptions
}  // namespace nftgen
