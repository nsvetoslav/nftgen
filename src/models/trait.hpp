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
    void set_generation_chance(double &generation_chance);
    const double get_generation_chance [[nodiscard]] () const;

    void set_trait_directory_id(int trait_folder_id);
    int get_trait_directory_id [[nodiscard]] () const;

    void set_trait_id(int trait_id);
    const int get_trait_id [[nodiscard]] () const;

    const cv::Mat &get_matrix [[nodiscard]] () const;
    void set_matrix(const cv::Mat &matrix);

    void set_rarity(const Rarities rarity);
    Rarities get_rarity [[nodiscard]] () const;

    std::string_view get_path [[nodiscard]] () const;
    void set_path(const std::string &path);

    std::string_view get_filename [[nodiscard]] () const;
    void set_filename(const std::string &filename);

    std::string_view get_directory_name [[nodiscard]] () const;
    void set_directory_name(const std::string &directory_name);

    static std::pair < std::optional<Trait>, GenerationResult> get_next_trait [[nodiscard]] (int trait_folder_id);

    //bool meets_all_exceptions(const Exceptions& exceptions);

private:
    int _trait_id{};
    std::string _path;
    std::string _filename;
    std::string _directory_name;
    double _generation_chance{};
    bool _is_excluded{};
    int _trait_folder_id{};
    Rarities _rarity{};
    cv::Mat _image_matrix;
};

}  // namespace nftgen
