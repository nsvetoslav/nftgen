#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Trait {
public:
	enum Rarities { Legendary, Epic, Rare, Uncommon, Common };

	Trait(std::string path, std::string file_name, std::string directory_name)
		: _generationChance(0.0), _isExcluded(false), _path(path), _file_name(file_name),
		  _directory_name(directory_name) {}

	Trait(const Trait &other)
		: _generationChance(other._generationChance), _isExcluded(other._isExcluded), _path(other._path),
		  _traitFolderId(other._traitFolderId), _traitId(other._traitId), _file_name(other._file_name),
		  _directory_name(other._directory_name) {}

	Trait &operator=(const Trait &other) {
		if (this != &other) {
			_generationChance = other._generationChance;
			_isExcluded = other._isExcluded;
			_path = other._path;
			_traitFolderId = other._traitFolderId;
			_traitId = other._traitId;
			_file_name = other._file_name;
			_directory_name = other._directory_name;
		}
		return *this;
	}

	virtual ~Trait() = default;

public:
	inline void set_generation_chance(double &generationChance) { _generationChance = generationChance; }

	inline double get_generation_chance [[nodiscard]] () { return _generationChance; }

	inline void set_trait_folder_id(int id) { _traitFolderId = id; }
	inline int	get_trait_folder_id [[nodiscard]] () const { return _traitFolderId; }

	inline void set_trait_id(int traitId) { _traitId = traitId; }
	const int	get_trait_id [[nodiscard]] () const { return _traitId; }

	std::optional<Trait> get_next_trait [[nodiscard]] () const;

	static inline size_t get_unix_time [[nodiscard]] () {
		auto time = std::chrono::system_clock::now();
		return time.time_since_epoch().count();
	}

	inline void		set_rarity(const Rarities rarity) { _rarity = rarity; }
	inline Rarities get_rarity [[nodiscard]] () const { return _rarity; }

	inline std::string_view get_path [[nodiscard]] () const { return _path; }
	inline std::string_view get_filename [[nodiscard]] () const { return _file_name; }
	inline std::string_view get_directory_name [[nodiscard]] () const { return _directory_name; }

private:
	int			_traitId{};
	std::string _path;
	std::string _file_name;
	std::string _directory_name;
	double		_generationChance{};
	bool		_isExcluded{};
	int			_traitFolderId{};
	Rarities	_rarity{};
};
