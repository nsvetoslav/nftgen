#pragma once

#include <string>
#include <vector>

#include "trait.hpp"
#include "exceptions.hpp"
#include "trait.hpp"

namespace nftgen
{

class TraitFolderException
{
public:
    TraitFolderException(int exceptioned_trait_directory_id, nftgen::ExceptionsTypes exception_type)
        : _exception_type(exception_type), _exceptioned_trait_directory_id(exceptioned_trait_directory_id)
    {
    }

    virtual ~TraitFolderException() = default;

private:
    int _exceptioned_trait_directory_id{};
    nftgen::ExceptionsTypes _exception_type{};
};

struct TraitDirectory
{
public:
    void set_path(std::string path);
    std::string_view get_path [[nodiscard]] ();

    std::string get_directory_name [[nodiscard]] ();
    void set_directory_name(std::string directory_name);

    void set_traits(std::vector<std::pair<std::string /*path*/, std::string> /*file_name*/> traits, std::string direcotry_name, int &directory_index);
    std::vector<nftgen::Trait> &get_traits [[nodiscard]] ();

    const int &get_id [[nodiscard]] () const;
    void set_id(int id);

    const double get_generation_chance [[nodiscard]] () const;
    void set_generation_chance(double generation_chance);

    void add_exception(int trait_directory_id, nftgen::ExceptionsTypes exception_type);

private:
    std::string _path;
    std::string _directory_name;
    std::vector<nftgen::Trait> _traits{};
    std::vector<TraitFolderException> _exceptions;
    double _generation_chance{};
    int _id{};
};

}  // namespace nftgen
