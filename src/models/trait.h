#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#include <opencv2/opencv.hpp>
#else
#include <opencv4/opencv2/opencv.hpp>
#endif

namespace nftgen
{

enum Rarities
{
    Legendary,
    Epic,
    Rare,
    Uncommon,
    Common
};

enum GenerationResult
{
    SkippedFolder,
    SuccessfullyGenerated
};

struct Trait
{
public:
    void SetGenerationChance(double generation_chance);
    [[nodiscard]] const double GetGenerationChance() const;

    void SetDirectoryGenerationOrderID(int directoryID);
    [[nodiscard]] int GetDirectoryGenerationOrderID() const;

    void SetDirectoryID(int directoryID);
    [[nodiscard]] int GetDirectoryID() const;

    void SetTraitID(int trait_id);
    [[nodiscard]] const int GetTraitID() const;

    [[nodiscard]] const cv::Mat &GetMatrix() const;
    void SetMatrix(cv::Mat &matrix);

    void SetRarity(const Rarities rarity);
    [[nodiscard]] Rarities GetRarity() const;

    [[nodiscard]] std::string_view GetPath() const;
    void SetPath(const std::string &path);

    [[nodiscard]] std::string_view GetFilename() const;
    void SetFilename(const std::string &filename);

    [[nodiscard]] std::string_view GetDirectoryName() const;
    void SetDirectoryName(const std::string &directory_name);

    [[nodiscard]] static std::pair<Trait *, GenerationResult> GetNextTrait(int directoryID, bool doNotApplyGenerationChances = false);

private:
    int _traitID{};
    std::string _path;
    std::string _filename;
    std::string _directory_name;
    double _generationChance{};
    int _directoryGenerationOrderID{};
    int _directoryID{};
    Rarities _rarity{};
    cv::Mat _imageMatrix;
};

}  // namespace nftgen
