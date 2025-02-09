#pragma once

#include <memory>
#include "../models/exceptions.hpp"
#include "exception.hpp"
#include "trait_exception_processors.hpp"
#include "traitdirs_exception_processors.hpp"

namespace nftgen
{
namespace typedexceptions
{
class TraitExceptionsFactory
{
public:
    static std::unique_ptr<IExceptionProcessor> Get(NFT_Metadata &nftMetadata,
                                                    const TraitException &trait_exception,
                                                    const Trait &generated_trait,
                                                    Exceptions& allExceptions)
    {
        switch (trait_exception.exceptionType)
        {
            case ExceptionsTypes::NotGenerateTraitWithDirectory:
                return std::make_unique<NotGenerateTraitWithDirectoryExceptionProcessor>(
                    NotGenerateTraitWithDirectoryExceptionProcessor(nftMetadata,
                                                                    generated_trait,
                                                                    trait_exception));

            case ExceptionsTypes::GenerateTraitOnlyWithTraits:
                return std::make_unique<GenerateTraitOnlyWithTraitsExceptionProcessor>(
                    GenerateTraitOnlyWithTraitsExceptionProcessor(nftMetadata,
                                                                  generated_trait,
                                                                  trait_exception,
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
