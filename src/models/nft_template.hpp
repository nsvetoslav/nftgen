#pragma once

#include <boost/functional/hash.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include "trait.hpp"

namespace nftgen {

	using json = nlohmann::ordered_json;

	class Attribute {
	public:
		std::string trait_type;
		std::string value;

		json to_json() const;
		void from_json(const json &j);
	};

	class File {
	public:
		std::string uri;
		std::string type;

		json to_json() const;
		void from_json(const json &j);
	};

	class Creator {
	public:
		std::string address;
		int			share;

		json to_json() const;
		void from_json(const json &j);
	};

	class Collection {
	public:
		std::string name;
		std::string family;

		json to_json() const;
		void from_json(const json &j);
	};

	class NFT_Metadata {
	public:
		std::string			   name;
		std::string			   symbol;
		std::string			   description;
		int					   seller_fee_basis_points{};
		std::string			   image;
		std::string			   external_url;
		std::vector<Attribute> attributes;
		Collection			   collection;
		std::vector<File>	   files;
		std::string			   category;
		std::vector<Creator>   creators;

		json to_json() const;
		void from_json(const json &j);

		NFT_Metadata() = default;

		// Copy constructor without copying assets
		NFT_Metadata(const NFT_Metadata &other);

	public:
		void   addTrait(const Trait &trait);
		size_t generate_traits_hash() const;

	private:
		void addAttribute(const Trait &trait);

	private:
		std::vector<Trait> _traits;
	};

} // namespace nftgen
