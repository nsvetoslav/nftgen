#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace nftgen
{
enum class ExceptionsTypes
{
    NotGenerateWith = 0,
    GenerateOnlyWith = 1,
    HigherGenerationChance = 2,
    LowerGenerationChance = 3,
    GenerateLast = 4,
    RenderBetween,
    Undefined,
};

inline static const ExceptionsTypes exceptionStringToEnum(const std::string &e)
{
    const std::map<std::string, ExceptionsTypes> MyEnumStrings{
        {"NotGenerateWith", ExceptionsTypes::NotGenerateWith},
        {"GenerateOnlyWith", ExceptionsTypes::GenerateOnlyWith},
        {"HigherGenerationChance", ExceptionsTypes::HigherGenerationChance},
        {"LowerGenerationChance", ExceptionsTypes::LowerGenerationChance},
        {"GenerateLast", ExceptionsTypes::GenerateLast},
        {"RenderBetween", ExceptionsTypes::RenderBetween},
    };
    auto it = MyEnumStrings.find(e);
    return it == MyEnumStrings.end() ? ExceptionsTypes::Undefined : it->second;
}

class TraitDirectoryException
{
public:
    std::string directory;
    std::string exceptionnedTraitDirectory;
    ExceptionsTypes exceptionType;

    void from_json(const nlohmann::json &j)
    {
        if (j.contains("directory"))
            j.at("directory").get_to(directory);
        if (j.contains("exceptionnedTraitDirectory"))
            j.at("exceptionnedTraitDirectory").get_to(exceptionnedTraitDirectory);
        if (j.contains("exceptionType"))
        {
            std::string exception;
            j.at("exceptionType").get_to(exception);
            exceptionType = exceptionStringToEnum(exception);
        }
    }
};

class TraitException
{
public:
    std::string trait;
    std::string exceptionnedTraitDirectory;
    std::string exceptionnedTrait;
    ExceptionsTypes exceptionType;
    std::string exceptionnedDirectory;

    void from_json(const nlohmann::json &j)
    {
        if (j.contains("trait"))
            j.at("trait").get_to(trait);
        if (j.contains("exceptionnedTraitDirectory"))
            j.at("exceptionnedTraitDirectory").get_to(exceptionnedTraitDirectory);
        if (j.contains("exceptionnedTrait"))
            j.at("exceptionnedTrait").get_to(exceptionnedTrait);
        if (j.contains("exceptionType"))
        {
            std::string exception;
            j.at("exceptionType").get_to(exception);
            exceptionType = exceptionStringToEnum(exception);
        }
        if (j.contains("exceptionnedDirectory"))
            j.at("exceptionnedDirectory").get_to(exceptionnedDirectory);
    }
};

class Exceptions
{
public:
    std::vector<TraitDirectoryException> traitDirectoriesExceptions;
    std::vector<TraitException> traitsExceptions;

    // from_json with contains() checks for each vector
    void from_json(const nlohmann::json &j)
    {
        if (j.contains("onGeneratedTraitDirectoriesExceptions") && j["onGeneratedTraitDirectoriesExceptions"].is_array())
        {
            for (const auto &exception : j["onGeneratedTraitDirectoriesExceptions"])
            {
                TraitDirectoryException directoryException;
                directoryException.from_json(exception);
                traitDirectoriesExceptions.push_back(directoryException);
            }
        }

        if (j.contains("onGeneratedTraitsExceptions") && j["onGeneratedTraitsExceptions"].is_array())
        {
            for (const auto &exception : j["onGeneratedTraitsExceptions"])
            {
                TraitException traitException;
                traitException.from_json(exception);
                traitsExceptions.push_back(traitException);
            }
        }
    }
};

}  // namespace nftgen
