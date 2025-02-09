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
                   std::optional<std::vector<std::string>> apply_with_traits,
                   bool bSkipReverseCheck = false )
    {
        this->apply_from_trait = apply_from_trait;
        this->apply_from_trait_dir = apply_from_trait_dir;

        this->apply_to_trait = apply_to_trait;
        this->exceptionType = exceptionType;
        this->apply_to_directory = apply_to_directory;
        this->apply_with_traits = apply_with_traits;
        this->bSkipReverseCheck = bSkipReverseCheck;
    }

    TraitException(std::string apply_from_trait,
                   std::string apply_from_trait_dir,
                   std::string render_after,
                   ExceptionsTypes exceptionType)
    {
        this->apply_from_trait = apply_from_trait;
        this->apply_from_trait_dir = apply_from_trait_dir;
        this->exceptionType = exceptionType;
        this->render_after = render_after;
    }

    bool Compare(const TraitException &t2)
    {
        return apply_from_trait == t2.apply_from_trait &&
               apply_from_trait_dir == t2.apply_from_trait_dir &&
               apply_to_trait == t2.apply_to_trait &&
               apply_to_directory == t2.apply_to_directory &&
               exceptionType == t2.exceptionType &&
               render_after == t2.render_after &&
               apply_with_traits == t2.apply_with_traits;
    }

public:
    std::string apply_from_trait;
    std::string apply_from_trait_dir;

    std::string apply_to_trait;
    std::string apply_to_directory;
    std::optional<std::vector<std::string>> apply_with_traits;
    ExceptionsTypes exceptionType;
    std::string render_after;
    bool bSkipReverseCheck = false;
};

class Exceptions
{
public:
    std::vector<TraitDirectoryException> traitDirectoriesExceptions;
    std::vector<TraitException> traitsExceptions;
};

}  // namespace nftgen
