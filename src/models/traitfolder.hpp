#pragma once

#include <string>
#include <vector>

#include "trait.hpp"

class TraitFolder {
public:
	TraitFolder(std::string path) : _generationChance(0.0), _path(path) {}

	virtual ~TraitFolder() = default;

public:
	inline void setPath(std::string path) { _path = path; }
	inline void set_directory_name(std::string directory_name) { _directory_name = directory_name; }

	void setTraits(std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traits,
				   std::string															  direcotry_name);

	inline const int &getId() { return _id; }

	inline void setId(int id) { _id = id; }

	inline std::vector<Trait> &getTraits() { return _traits; }

	inline std::string_view get_path [[nodiscard]] () { return _path; }

	inline std::string get_directory_name [[nodiscard]] () { return _directory_name; }

private:
	std::string		   _path;
	std::string		   _directory_name;
	std::vector<Trait> _traits{};
	double			   _generationChance{};
	int				   _id{};
};
