#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace nftgen
{
enum class ExceptionsTypes
{
    Undefined = -1,
    NotGenerateTraitWithDirectory = 1,
    GenerateTraitOnlyWithTraits = 2,
    NotGenerateDirectoryWithDirectory = 3,
    NotGenerateTraitsWithTraits = 4,
    RenderAfter = 5,
    RenderLast = 6
};

class TraitDirectoryException
{
public:
    TraitDirectoryException(std::string sourceDirectory,
                            std::string targetDirectory,
                            ExceptionsTypes exceptionType)
    {
        this->sourceDirectory = sourceDirectory;
        this->targetDirectory = targetDirectory;
        this->exceptionType = exceptionType;
    }

public:
    std::string sourceDirectory;
    std::string targetDirectory;
    ExceptionsTypes exceptionType;
};

class TraitException
{
public:
    TraitException(std::string sourceTrait,
                   std::string sourceDirectory,
                   std::string targetTrait,
                   std::string targetDirectory,
                   ExceptionsTypes exceptionType,
                   std::optional<std::vector<std::string>> tagetTraits,
                   bool bSkipReverseCheck = false)
    {
        this->sourceTrait = sourceTrait;
        this->sourceDirectory = sourceDirectory;

        this->targetTrait = targetTrait;
        this->exceptionType = exceptionType;
        this->targetDirectory = targetDirectory;
        this->targetTraits = tagetTraits;
        this->skipReverseCheck = bSkipReverseCheck;
    }

    TraitException(std::string sourceTrait,
                   std::string sourceDirectory,
                   std::string renderAfterDirectory,
                   ExceptionsTypes exceptionType)
    {
        this->sourceTrait = sourceTrait;
        this->sourceDirectory = sourceDirectory;
        this->exceptionType = exceptionType;
        this->renderAfterDirectory = renderAfterDirectory;
    }

    bool Compare(const TraitException &t2)
    {
        return sourceTrait == t2.sourceTrait &&
               sourceDirectory == t2.sourceDirectory &&
               targetTrait == t2.targetTrait &&
               targetDirectory == t2.targetDirectory &&
               exceptionType == t2.exceptionType &&
               renderAfterDirectory == t2.renderAfterDirectory &&
               targetTraits == t2.targetTraits;
    }

public:
    std::string sourceTrait;
    std::string sourceDirectory;

    std::string targetTrait;
    std::string targetDirectory;
    std::optional<std::vector<std::string>> targetTraits;
    ExceptionsTypes exceptionType;
    std::string renderAfterDirectory;
    bool skipReverseCheck = false;
};

class Exceptions
{
public:
    std::vector<TraitDirectoryException> traitDirectoriesExceptions;
    std::vector<TraitException> traitsExceptions;
};

}  // namespace nftgen
