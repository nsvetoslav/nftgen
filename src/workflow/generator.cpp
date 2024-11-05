#include "generator.hpp"

#include <filesystem>
#include <format>

#include "generator.hpp"
#include "settings.hpp"

using namespace nftgen;

std::vector<TraitFolder> generator::_traitsDirectories{};

bool generator::load_directories() {
	std::string			   root_path = _traitsDirectory;
	utilities::filemanager fileManager;

	if (!std::filesystem::exists(root_path) && !std::filesystem::is_directory(root_path)) {
		// logger.log(!fileManager.exists(), _traitsDirectory);
		return false;
	}

	// Traversing directories in assets
	std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> directoryPaths;
	fileManager.get_directory_files(root_path, directoryPaths);

	for (const auto &traitDirectory : directoryPaths) {
		const auto &traitDirectoryPath = traitDirectory.first;
		TraitFolder traitFolder(traitDirectoryPath);

		if (traitDirectory.second == "9. Extras") traitFolder.set_generation_chance(10);

		// All the trait files paths in a traitDirectory
		std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traits;
		fileManager.get_directory_files(traitDirectory.first, traits);

		for (auto &item : traits) {
			const auto &traitPath = item.first;
			_imagesMap[traitPath] = std::move(cv::imread(traitPath, cv::IMREAD_UNCHANGED));
		}

		traitFolder.setTraits(traits, traitDirectory.second);

		// mem moving traitFolder in _traitsDirectories
		_traitsDirectories.push_back(std::move(traitFolder));
	}

	nftgen::calculator::set_equal_geneartion_chances(_traitsDirectories);

	return true;
}

void generator::create_gen_directory(std::string_view dir) const {
	if (std::filesystem::exists(dir)) return;
	std::filesystem::create_directory(dir);
}

bool nftgen::generator::addGeneratedNft(nftgen::NFT_Metadata &nftMetadata) {
	size_t traitsHash = nftMetadata.generate_traits_hash();
	if (_nfts_hashes.find(traitsHash) != _nfts_hashes.end()) { return false; }

	_nfts_hashes.insert(std::move(traitsHash));
	_generatedNfts.push_back(std::move(nftMetadata));

	for (const auto &trait : nftMetadata.getTraits()) {
		_nfts_grouped_traits[std::make_pair(trait.get_trait_folder_id(), trait.get_trait_id())]++;
	}

	return true;
}

bool generator::generate(unsigned long nftsCount) {
	try {
		if (!load_directories()) { return false; }

		if (_traitsDirectories.empty()) { return false; }

		std::ifstream templateNFTMetadataFile(settings::get_instance().get_template_file());

		if (!templateNFTMetadataFile.is_open()) {
			std::cerr << "Could not open the templateNFTMetadata file!" << std::endl;
			return false;
		}

		json j;
		templateNFTMetadataFile >> j;
		_templateMetadata.from_json(j);

		int generatedNftCount = 0;
		while (generatedNftCount < nftsCount) {
			std::cout << "Starting " << generatedNftCount << "th nft generation." << std::endl;
			generate_single_nft(generatedNftCount);
		}

		std::vector<std::pair<std::pair<int, int>, size_t>> sorted_traits(_nfts_grouped_traits.begin(),
																		  _nfts_grouped_traits.end());

		// Step 2: Sort the vector by traitFolderID
		std::sort(sorted_traits.begin(), sorted_traits.end(), [](const auto &a, const auto &b) {
			return a.first.first < b.first.first; // Sort by traitFolderID
		});

		// Output header
		std::cout << "TraitFolderID | TraitID | Count\n";
		std::cout << "------------------------------------\n";

		// Step 3: Output the sorted traits
		for (const auto &entry : sorted_traits) {
			int	   traitFolderID = entry.first.first; // Folder ID
			int	   traitID = entry.first.second;	  // Trait ID
			size_t count = entry.second;			  // Count of times this trait was generated

			std::cout << traitFolderID << "            | " << traitID << "      | " << count << '\n';
		}

	} catch (const std::exception &exception) {
		// logger.log(exception);
		return false;
	}

	return true;
}

void nftgen::generator::generate_single_nft(int &generatedNftCount) {
	std::optional<Trait> first_trait = generate_first_random_trait();

	int i{};

	cv::Mat baseLayer = _imagesMap[std::string(first_trait->get_path())];

	baseLayer = convert_to_rgba(baseLayer);

	cv::Mat res(baseLayer.size(), baseLayer.type());

	// copying data from template without the assets
	NFT_Metadata nftMetadata(_templateMetadata);
	int			 traitDirIndex = 0;
	do {
		if (!first_trait.has_value()) {
			traitDirIndex++;
			continue;
		}

		auto currentImagePath = std::string(first_trait->get_path());

		cv::Mat frontLayer = _imagesMap[currentImagePath];
		if (frontLayer.empty()) {
			std::cerr << "Error loading front layer image: " << currentImagePath << std::endl;
			continue;
		}

		frontLayer = convert_to_rgba(frontLayer);
		alpha_composite(baseLayer, frontLayer, res);
		baseLayer = res.clone();

		first_trait = first_trait->get_next_trait();

		if (first_trait != std::nullopt) nftMetadata.addTrait(first_trait.value());

		traitDirIndex++;

	} while (traitDirIndex < _traitsDirectories.size() - 1);

	std::string directory = nftgen::settings::get_instance().get_generated_nfts_directory();
	create_gen_directory(directory);

	// if the generated nft hasn't been generated before, we increase the  generated nfts count
	if (addGeneratedNft(nftMetadata)) {
		generatedNftCount++;

		const std::string generatedNftNumber = std::to_string(generatedNftCount);
		// save the generated nft
		cv::imwrite(directory + "/" + generatedNftNumber + ".png", res);
		// save the nft metadata
		std::ofstream outFile(directory + "/" + generatedNftNumber + ".json", std::ios::trunc);
		if (outFile.is_open()) {
			outFile << nftMetadata.to_json().dump(4);
			outFile.close();
		} else {
			std::cout << "Failed to save nft" + generatedNftNumber + "metadata" << std::endl;
		}
	} else {
		std::cout << "Skipping already generated nft with hash:" + std::to_string(nftMetadata.generate_traits_hash())
				  << std::endl;
	}
}

void nftgen::generator::set_generation_chances(TraitFolder &traitFolder) {
	nftgen::calculator calculator;
	calculator.set_equal_geneartion_chances(traitFolder.getTraits());
}

cv::Mat generator::convert_to_rgba(const cv::Mat &input) {
	cv::Mat output;

	if (input.channels() == 4) {
		// If already RGBA, just return a copy
		output = input.clone();
	} else {
		// Convert to RGBA
		cv::cvtColor(input, output, cv::COLOR_BGR2BGRA); // BGR to BGRA
	}

	return output;
}

void generator::alpha_composite(const cv::Mat &baseLayer, const cv::Mat &frontLayer, cv::Mat &res) {
	CV_Assert(baseLayer.size() == frontLayer.size());
	CV_Assert(baseLayer.type() == CV_8UC4 && frontLayer.type() == CV_8UC4);
	res.create(baseLayer.size(), baseLayer.type());

	const uchar *basePtr = baseLayer.data;
	const uchar *frontPtr = frontLayer.data;
	uchar		*resPtr = res.data;

	int numPixels = baseLayer.rows * baseLayer.cols;
	for (int i = 0; i < numPixels; i++) {
		float alpha_B = frontPtr[i * 4 + 3] / 255.0f;
		if (alpha_B == 0 && frontPtr[i * 4 + 2] == 0 && frontPtr[i * 4 + 1] == 0) continue;

		for (int c = 0; c < 3; c++) {
			resPtr[i * 4 + c] = static_cast<uchar>(alpha_B * frontPtr[i * 4 + c] + (1 - alpha_B) * basePtr[i * 4 + c]);
		}
		resPtr[i * 4 + 3] = 255;
	}
}

std::optional<Trait> generator::generate_first_random_trait [[nodiscard]] () {
	std::sort(_traitsDirectories.begin(), _traitsDirectories.end(), [](auto &dir1, auto &dir2) {
		return dir1.get_path() < dir2.get_path();
	});

	for (int i = 0; i < _traitsDirectories.size(); i++) {
		_traitsDirectories[i].setId(i);

		for (auto &trait : _traitsDirectories[i].getTraits()) { trait.set_trait_folder_id(i); }
	}

	auto time = Trait::get_unix_time();

	std::srand(time);

	return _traitsDirectories[0].getTraits()[std::rand() % _traitsDirectories[0].getTraits().size()];
}
