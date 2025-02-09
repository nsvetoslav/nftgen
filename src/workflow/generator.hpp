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
    generator(std::string traitDirectoriesRootPath);
    virtual ~generator() = default;

public:
    [[nodiscard]] bool generate(unsigned long nftsCount);
    static std::vector<TraitDirectory> _traitsDirectories;

private:
    void GenerateSingleNFT(int &generatedNftCount);
    [[nodiscard]] bool LoadDirectories();
    void SetGenerationChances(TraitDirectory &traitDirectory);
    [[nodiscard]] cv::Mat ConvertToRgba(const cv::Mat &input);
    void AlphaComposite(const cv::Mat &baseLayer, const cv::Mat &frontLayer, cv::Mat &res);
    void CreateGenDirectory(std::string_view dir) const;
    [[nodiscard]] bool AddGeneratedNFT(nftgen::NFT_Metadata &nftMetadata);
    void CheckTraitDirectoryExceotions(NFT_Metadata &nftMetadata, Trait &currentTrait, bool &continueNextDir, bool &continueNextTrait);
    void CheckCrossedExceptions(Trait &currentGeneratedTrait, NFT_Metadata &nftMetadata, bool &continueToNextTrait);
    void CheckTraitExceotions(NFT_Metadata &nftMetadata, Trait &currentTrait, bool &continueNextDir, bool &continueNextTrait);

private:
    void ProcessNFTs(int start_index, int end_index, std::vector<NFT_Metadata> &_generatedNFTs);
    void SaveNTFSInParrallel(std::vector<NFT_Metadata> &_generatedNFTs);
    void MoveElement(std::vector<Trait> &arr, size_t fromIndex, size_t toIndex);

private:
    std::atomic<int> _progressCounter{0};
    Exceptions _exceptions;
    std::string _generatedNFTsDirectory;
    std::string _traitsDirectoriesRootPath;
    nftgen::NFT_Metadata _nftTemplateMetadata;
    std::vector<nftgen::NFT_Metadata> _generatedNFTsMetadatas;
    std::unordered_set<size_t> _nftsHashes;
    std::unordered_map<std::pair<int /*trait_directory_id*/, int /*trait_id*/>, size_t, boost::hash<std::pair<int, int>>> _groupedNFTTraits;
    std::mutex consoleMutex;
};
}  // namespace nftgen
