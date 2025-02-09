#pragma once

#include <iostream>
#include <optional>

#ifdef _WIN32
#include <opencv2/opencv.hpp>
#else
#include <opencv4/opencv2/opencv.hpp>
#endif

#include <unordered_set>
#include "../models/nft_template.hpp"
#include "../models/traitfolder.hpp"
#include "calculator.hpp"
#include "filemanager.hpp"

namespace nftgen
{
class generator
{
public:
    generator(std::string trait_directories_root_path);
    virtual ~generator() = default;

public:
    [[nodiscard]] bool generate(unsigned long nfts_count);
    static std::vector<TraitDirectory> _traits_directories;

private:
    void generate_single_nft(int &generated_nft_count);
    [[nodiscard]] bool load_directories();
    void set_generation_chances(TraitDirectory &trait_directory);
    [[nodiscard]] cv::Mat convert_to_rgba(const cv::Mat &input);
    void alpha_composite(const cv::Mat &base_layer, const cv::Mat &front_layer, cv::Mat &res);
    void create_gen_directory(std::string_view dir) const;
    [[nodiscard]] bool add_generated_nft(nftgen::NFT_Metadata &nft_metadata);
    void CheckTraitDirectoryExceotions(NFT_Metadata &nft_metadata, Trait &current_trait, bool &bContinueNextDir, bool &bContinueNextTrait);
    void CheckCrossedExceptions(Trait &currentGeneratedTrait, NFT_Metadata& nftMetadata, bool& continueToNextTrait);
    void CheckTraitExceotions(NFT_Metadata &nft_metadata, Trait &current_trait, bool &bContinueNextDir, bool &bContinueNextTrait);

private:
    void process_nfts(int start_index, int end_index, std::vector<NFT_Metadata> &_generated_nfts);
    void save_nfts_in_parallel(std::vector<NFT_Metadata> &_generated_nfts);
    void moveElement(std::vector<Trait> &arr, size_t fromIndex, size_t toIndex);

private:
    std::atomic<int> progress_counter{0};
    Exceptions _exceptions;
    std::string _generated_nfts_directory;
    std::string _traits_directories_root_path;
    nftgen::NFT_Metadata _nft_template_metadata;
    std::vector<nftgen::NFT_Metadata> _generated_nfts_metadatas;
    std::unordered_set<size_t> _nfts_hashes;
    std::unordered_map<std::pair<int /*trait_directory_id*/, int /*trait_id*/>, size_t, boost::hash<std::pair<int, int>>> _nfts_grouped_traits;
    std::mutex console_mutex;
};
}  // namespace nftgen
