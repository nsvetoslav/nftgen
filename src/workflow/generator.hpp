#pragma once

#include <iostream>
#include <optional>

#ifdef _WIN32
#include <opencv2/opencv.hpp>
#else
#include <opencv4/opencv2/opencv.hpp>
#endif

#include "../models/nft_template.hpp"
#include "../models/traitfolder.hpp"
#include "calculator.hpp"
#include "filemanager.hpp"

namespace nftgen {
    class generator {
    public:
        generator(std::string traitsDirectory) noexcept : _traitsDirectory(traitsDirectory) {}

        virtual ~generator() = default;

    public:
        bool                            generate(unsigned long nftsCount);
        static std::vector<TraitFolder> _traitsDirectories;

    private:
        void                 generate_sigle_nft(const int &nftNumber);
        bool                 load_directories [[nodiscard]] ();
        std::optional<Trait> generate_first_random_trait [[nodiscard]] ();
        void                 set_generation_chances(TraitFolder &traitFolder);
        cv::Mat              convert_to_rgba [[nodiscard]] (const cv::Mat &input);
        void                 alpha_composite(const cv::Mat &baseLayer, const cv::Mat &frontLayer, cv::Mat &res);
        void                 create_gen_directory(std::string_view dir) const;

    private:
        std::string                    _traitsDirectory;
        std::map<std::string, cv::Mat> _imagesMap;
        nftgen::NFT_Metadata           _templateMetadata;
    };
} // namespace nftgen
