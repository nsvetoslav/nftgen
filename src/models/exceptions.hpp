#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace nftgen
{
enum class ExceptionsTypes
{
    Undefined = -1,
    NotGenerateTraitWithDirectory = 0,
    GenerateOnlyWith = 1,
    HigherGenerationChance = 2,
    LowerGenerationChance = 3,
    GenerateLast = 4,
    RenderTraitBetweenDirectories = 5,
    GenerateTraitOnlyWithTraits = 6,
    NotGenerateDirectoryWithDirectory = 7,
};

class TraitDirectoryException
{
public:
    TraitDirectoryException(std::string apply_from_directory, std::string apply_to_directory, ExceptionsTypes exceptionType)
    {
        this->apply_from_directory = apply_from_directory;
        this->apply_to_directory = apply_to_directory;
        this->exceptionType = exceptionType;
    }

public:
    std::string apply_from_directory;
    std::string apply_to_directory;
    ExceptionsTypes exceptionType;
};

class TraitException
{
public:
    TraitException(std::string apply_from_trait,
                   std::string apply_from_trait_dir,
                   std::string apply_to_trait,
                   std::string apply_to_directory,
                   ExceptionsTypes exceptionType,
                   std::optional<std::vector<std::string>> apply_with_traits)
    {
        this->apply_from_trait = apply_from_trait;
        this->apply_from_trait_dir = apply_from_trait_dir;

        this->apply_to_trait = apply_to_trait;
        this->exceptionType = exceptionType;
        this->apply_to_directory = apply_to_directory;
        this->apply_with_traits = apply_with_traits;
    }

public:
    std::string apply_from_trait;
    std::string apply_from_trait_dir;

    std::string apply_to_trait;
    std::string apply_to_directory;
    std::optional<std::vector<std::string>> apply_with_traits;
    ExceptionsTypes exceptionType;
};

class Exceptions
{
public:
    std::vector<TraitDirectoryException> traitDirectoriesExceptions;
    std::vector<TraitException> traitsExceptions;
};

}  // namespace nftgen
