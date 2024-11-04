#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class Trait {
public:
    Trait(std::string path) : _generationChance(0.0), _isExcluded(false), _path(path) {}

    Trait(const Trait &other)
        : _generationChance(other._generationChance), _isExcluded(other._isExcluded), _path(other._path),
          _traitFolderId(other._traitFolderId) {}

    Trait &operator=(const Trait &other) {
        if (this != &other) {
            _generationChance = other._generationChance;
            _isExcluded = other._isExcluded;
            _path = other._path;
            _traitFolderId = other._traitFolderId;
        }
        return *this;
    }

    virtual ~Trait() = default;

public:
    inline void set_generation_chance(double &generationChance) { _generationChance = generationChance; }

    inline double get_generation_chance [[nodiscard]] () { return _generationChance; }

    inline void set_trait_folder_id(int id) { _traitFolderId = id; }

    std::optional<Trait> get_next_trait [[nodiscard]] () const;

    static inline size_t get_unix_time [[nodiscard]] () {
        auto time = std::chrono::system_clock::now();
        return time.time_since_epoch().count();
    }

    inline std::string_view get_path [[nodiscard]] () { return _path; }

private:
    int         traitFolderId{};
    std::string _path;
    double      _generationChance{};
    bool        _isExcluded{};
    int         _traitFolderId{};
};
