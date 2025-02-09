#pragma once

#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include "../models/IExceptionProcessor.hpp"
#include "../models/trait.h"
#include "IExceptionProcessor.hpp"
#include "../models/NFTTemplateMetadata.h"

namespace nftgen
{
namespace typedexceptions
{

class BaseTraitExceptionProcessor : public IExceptionProcessor
{
public:
    BaseTraitExceptionProcessor(NFT_Metadata &nftMetadata, const Trait &generatedTrait, const TraitException &traitException) noexcept
        :
        _generatedTrait(generatedTrait), _traitException(traitException), _nftMetadata(nftMetadata)
    {
    }

    virtual ~BaseTraitExceptionProcessor() noexcept = default;

public:
    [[nodiscard]] inline bool MeetsGenerationConditions() override
    {
        return MeetsGenerationConditions(_nftMetadata, _generatedTrait, _traitException);
    }

    inline bool ContinueToNextDirectory() const override
    {
        return continueToNextDirectory;
    }

    inline bool ContinueToNextTrait() const override
    {
        return continueToNextTrait;
    }

private:
    [[nodiscard]] virtual inline bool MeetsGenerationConditions(NFT_Metadata &nftMetadata, const Trait &trait, const TraitException context)
    {
        for (Trait &metadataTrait : nftMetadata.get_traits())
        {
            if (!CheckMetadataTrait(metadataTrait))
                return false;
        }

        return true;
    }

protected:
    [[nodiscard]] virtual bool CheckMetadataTrait(const Trait &metadataTrait) = 0;

protected:
    [[nodiscard]] inline const NFT_Metadata &GetNftMetadata() const
    {
        return _nftMetadata;
    }

    [[nodiscard]] inline const Trait &GetCurrentGeneratedTrait() const
    {
        return _generatedTrait;
    }

    [[nodiscard]] inline const TraitException &GetTraitException() const
    {
        return _traitException;
    }

    inline void SetContinueToNextTrait(bool continueToNextTrait)
    {
        this->continueToNextTrait = continueToNextTrait;
    }

    inline void SetContinueToNextDirectory(bool continueToNextDirectory)
    {
        this->continueToNextDirectory = continueToNextDirectory;
    }

private:
    NFT_Metadata &_nftMetadata;
    const Trait &_generatedTrait;
    const TraitException _traitException;

    bool continueToNextTrait{};
    bool continueToNextDirectory{};
};

class BaseTraitDirectoryExceptionProcessor : public IExceptionProcessor
{
public:
    BaseTraitDirectoryExceptionProcessor(NFT_Metadata &nftMetadata,
                                         const Trait &generatedTrait,
                                         const TraitDirectoryException &traitDirectoryException) noexcept
        :
        _generatedTrait(generatedTrait), _traitDirectoryException(traitDirectoryException), _nftMetadata(nftMetadata)
    {
    }

    virtual ~BaseTraitDirectoryExceptionProcessor() noexcept = default;

public:
    [[nodiscard]] inline bool MeetsGenerationConditions() override
    {
        return MeetsGenerationConditions(_nftMetadata, _generatedTrait, _traitDirectoryException);
    }

    inline bool ContinueToNextDirectory() const override
    {
        return continueToNextDirectory;
    }

    inline bool ContinueToNextTrait() const override
    {
        return continueToNextTrait;
    }

private:
    [[nodiscard]] virtual inline bool MeetsGenerationConditions(NFT_Metadata &nftMetadata,
                                                                const Trait &trait,
                                                                const TraitDirectoryException context)
    {
        for (Trait &metadataTrait : nftMetadata.get_traits())
        {
            if (!CheckMetadataTrait(metadataTrait))
                return false;
        }

        return true;
    }

protected:
    [[nodiscard]] virtual bool CheckMetadataTrait(const Trait &metadataTrait) = 0;

protected:
    [[nodiscard]] inline const NFT_Metadata &GetNftMetadata() const
    {
        return _nftMetadata;
    }

    [[nodiscard]] inline const Trait &GetCurrentGeneratedTrait() const
    {
        return _generatedTrait;
    }

    [[nodiscard]] inline const TraitDirectoryException &GetTraitDirectoryException() const
    {
        return _traitDirectoryException;
    }

    inline void SetContinueToNextTrait(bool continueToNextTrait)
    {
        this->continueToNextTrait = continueToNextTrait;
    }

    inline void SetContinueToNextDirectory(bool continueToNextDirectory)
    {
        this->continueToNextDirectory = continueToNextDirectory;
    }

private:
    NFT_Metadata &_nftMetadata;
    const Trait &_generatedTrait;
    const TraitDirectoryException _traitDirectoryException;

    bool continueToNextTrait{};
    bool continueToNextDirectory{};
};

}  // namespace typedexceptions
}  // namespace nftgen
