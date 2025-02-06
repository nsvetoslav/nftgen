#pragma once

#include <string>
#include <vector>

#include "trait.hpp"
#include "exceptions.hpp"
#include "trait.hpp"

namespace nftgen
{
struct TraitDirectory
{
public:
    void set_path(std::string path);
    std::string_view get_path [[nodiscard]] ();

    std::string get_directory_name [[nodiscard]] ();
    void set_directory_name(std::string directory_name);

    void set_traits(std::vector<std::pair<std::string /*path*/, std::string> /*file_name*/> traits, std::string direcotry_name, int &gen_directory_index, int &directory_index);
    std::vector<nftgen::Trait> &get_traits [[nodiscard]] ();

    const int &get_id [[nodiscard]] () const;
    void set_id(int id);

    const double get_generation_chance [[nodiscard]] () const;
    void set_generation_chance(double generation_chance);

    [[nodiscard]] int get_gen_order() const;
    void set_gen_order(int gen_order);

private:
    std::string _path;
    std::string _directory_name;
    std::vector<nftgen::Trait> _traits{};
    double _generation_chance{};
    int _generation_order{};
    int _id{};    
};

}  // namespace nftgen
