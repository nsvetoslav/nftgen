#pragma once

#include <boost/functional/hash.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "trait.hpp"

namespace nftgen
{

using json = nlohmann::ordered_json;

class Attribute
{
public:
    json to_json() const;
    void from_json(const json &j);

public:
    std::string trait_type;
    std::string value;
};

class File
{
public:
    json to_json() const;
    void from_json(const json &j);

public:
    std::string uri;
    std::string type;
};

class Creator
{
public:
    json to_json() const;
    void from_json(const json &j);

public:
    std::string address;
    int share{};
};

class Collection
{
public:
    json to_json() const;
    void from_json(const json &j);

public:
    std::string name;
    std::string family;
};

class NFT_Metadata
{
public:
    NFT_Metadata() = default;
    NFT_Metadata(const NFT_Metadata &other);

public:
    json to_json() const;
    void from_json(const json &j);

public:
    const std::vector<Trait> &get_traits() const;
    void sort_traits();
    void add_trait(Trait &trait);
    size_t generate_traits_hash() const;

private:
    void add_attribute_by_trait(const Trait &trait);

public:
    std::string name;
    std::string symbol;
    std::string description;
    int seller_fee_basis_points{};
    std::string image;
    std::string external_url;
    std::vector<Attribute> attributes;
    Collection collection;
    std::vector<File> files;
    std::string category;
    std::vector<Creator> creators;

private:
    std::vector<Trait> _traits;
    std::vector<cv::Mat> _images_matrixes;
};

}  // namespace nftgen
