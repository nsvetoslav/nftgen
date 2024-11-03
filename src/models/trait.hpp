#pragma  once

#include <cstddef>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

class Trait
{
public:
    Trait(std::string path)
        : _generationChance(0.0),
          _isExcluded(false),
        _path(path)
    {
    }

    virtual ~Trait() = default;

public:
    inline void setGenerationChance [[nodiscard]] (double& generationChance)
    {
        _generationChance = generationChance;
    }

    inline double getGenerationChance [[nodiscard]] ()
    {
        return _generationChance;
    }

    inline void setTraitFolderId [[nodiscard]] (int id)
    {
        _traitFolderId = id;
    }

    std::optional<Trait> get_next_trait [[nodiscard]] () const;

    static inline size_t get_unix_time [[nodiscard]]() {
	auto time = std::chrono::system_clock::now();
	return time.time_since_epoch().count();
    }

    inline std::string_view get_path [[nodiscard]]() {
	return _path;
    }

    int traitFolderId;
private:
    std::string _path;
    double _generationChance{};
    bool _isExcluded{};
    int _traitFolderId{};
};
