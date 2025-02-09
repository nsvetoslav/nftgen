#pragma once

#include <memory>
#include "../models/IExceptionProcessor.hpp"
#include "IExceptionProcessor.hpp"
#include "TraitExceptionsProcessors.hpp"
#include "DirectoriesExceptionsProcessor.hpp"

namespace nftgen
{
namespace typedexceptions
{
class TraitExceptionsFactory
{
public:
    static std::unique_ptr<IExceptionProcessor> Get(NFT_Metadata &nftMetadata,
                                                    const TraitException &traitException,
                                                    const Trait &generatedTrait,
                                                    Exceptions& allExceptions)
    {
        switch (traitException.exceptionType)
        {
            case ExceptionsTypes::NotGenerateTraitWithDirectory:
                return std::make_unique<NotGenerateTraitWithDirectoryExceptionProcessor>(
                    NotGenerateTraitWithDirectoryExceptionProcessor(nftMetadata,
                                                                    generatedTrait,
                                                                    traitException));

            case ExceptionsTypes::GenerateTraitOnlyWithTraits:
                return std::make_unique<GenerateTraitOnlyWithTraitsExceptionProcessor>(
                    GenerateTraitOnlyWithTraitsExceptionProcessor(nftMetadata,
                                                                  generatedTrait,
                                                                  traitException,
                                                                  allExceptions));

            default:
                return nullptr;
        }
    }
};

class TraitDirectoriesExceptionsFactory
{
public:
    static std::unique_ptr<IExceptionProcessor> Get(NFT_Metadata &nftMetadata,
                                                    const TraitDirectoryException &trait_directory_exception,
                                                    const Trait &generated_trait)
    {
        switch (trait_directory_exception.exceptionType)
        {
            case ExceptionsTypes::NotGenerateDirectoryWithDirectory:
                return std::make_unique<NotGenerateDirectoryWithDirectoryExceptionProcessor>(
                    NotGenerateDirectoryWithDirectoryExceptionProcessor(nftMetadata,
                                                                        generated_trait,
                                                                        trait_directory_exception));

            default:
                return nullptr;
        }
    }
};

}  // namespace typedexceptions
}  // namespace nftgen
