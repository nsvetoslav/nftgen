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

namespace nftgen {
	class generator {

	public:
		generator(std::string traitsDirectory) noexcept : _traitsDirectory(traitsDirectory) {}

		virtual ~generator() = default;

	public:
		bool							generate(unsigned long nftsCount);
		static std::vector<TraitFolder> _traitsDirectories;

	private:
		void				 generate_single_nft(int &generatedNftCount);
		bool				 load_directories [[nodiscard]] ();
		std::optional<Trait> generate_first_random_trait [[nodiscard]] ();
		void				 set_generation_chances(TraitFolder &traitFolder);
		cv::Mat				 convert_to_rgba [[nodiscard]] (const cv::Mat &input);
		void				 alpha_composite(const cv::Mat &baseLayer, const cv::Mat &frontLayer, cv::Mat &res);
		void				 create_gen_directory(std::string_view dir) const;
		bool				 addGeneratedNft(nftgen::NFT_Metadata &nftMetadata);

	private:
		std::string						  _traitsDirectory;
		std::map<std::string, cv::Mat>	  _imagesMap;
		nftgen::NFT_Metadata			  _templateMetadata;
		std::vector<nftgen::NFT_Metadata> _generatedNfts;
		std::unordered_set<size_t>		  _nfts_hashes;
		std::unordered_map<std::pair<int /*traitFolderID*/, int /*traitID*/>, size_t, boost::hash<std::pair<int, int>>>
			_nfts_grouped_traits;
	};
} // namespace nftgen
