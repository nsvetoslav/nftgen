#include "generator.hpp"
#include "generator.hpp"
#include <filesystem>
#include <format>
#include "settings.hpp"

using namespace nftgen;

std::vector<TraitFolder> generator::_traitsDirectories{};

bool generator::loadDirectories()
{
	std::string root_path = _traitsDirectory;
	utilities::filemanager fileManager;

	if (!std::filesystem::exists(root_path) && !std::filesystem::is_directory(root_path))
	{
		// logger.log(!fileManager.exists(), _traitsDirectory);
		return false;
	}

	// Traversing directories in assets
	std::vector<std::string> directoryPaths;
	fileManager.getDirectoryFilePaths(root_path, directoryPaths);

	for (const auto& traitDirectory : directoryPaths) {
		TraitFolder traitFolder(traitDirectory);

		// All the trait files paths in a traitDirectory
		std::vector<std::string> traits;
		fileManager.getDirectoryFilePaths(traitDirectory, traits);

		traitFolder.setTraits(traits);

		// mem moving traitFolder in _traitsDirectories
		_traitsDirectories.push_back(std::move(traitFolder));
	}

	return true;
}

void generator::create_gen_directory(std::string_view dir) const {
	if(std::filesystem::exists(dir))
		return;
	std::filesystem::create_directory(dir);
}

bool generator::generate() {
	try
	{
		if (!loadDirectories())
		{
			return false;
		}

		if (_traitsDirectories.empty())
		{
			return false;
		}

		std::optional<Trait> first_trait = generate_first_random_trait();

		int i{};	
		cv::Mat baseLayer = cv::imread(std::string(first_trait->get_path()), cv::IMREAD_UNCHANGED);
		baseLayer = convertToRGBA(baseLayer);
		cv::Mat res(baseLayer.size(), baseLayer.type());

		do {	
			if(i != 0) {
				auto curr = std::string(first_trait->get_path());
				cv::Mat frontLayer = cv::imread(curr, cv::IMREAD_UNCHANGED);
				if (frontLayer.empty()) {
					std::cerr << "Error loading front layer image: " << curr << std::endl;
					continue;
				}
				frontLayer = convertToRGBA(frontLayer);
				alphaComposite(baseLayer, frontLayer, res);
				baseLayer = res.clone(); 
			}
			first_trait = first_trait->get_next_trait();
			i++;
		} while (first_trait->get_next_trait().has_value());

		// TODO: Some better name XD
		std::string directory = nftgen::settings::getInstance().get_generated_nfts_directory();
		create_gen_directory(directory);
		std::string generatedImageName = directory + "/" + std::to_string(first_trait->get_unix_time()) + ".png";
		cv::imwrite(generatedImageName, res);

		// Setting generation chances for the first prioritized traitDirectory
		setGenerationChacnes(_traitsDirectories.front());
	}
	catch (const std::exception& exception)
	{
		// logger.log(exception); 
		return false;
	}

	return true;
}

void nftgen::generator::setGenerationChacnes(TraitFolder& traitFolder)
{
	nftgen::calculator calculator;
	calculator.setEqualGenerationChances(traitFolder.getTraits());
}

cv::Mat generator::convertToRGBA(const cv::Mat& input)
{
	cv::Mat output;

	if (input.channels() == 4) {
		// If already RGBA, just return a copy
		output = input.clone();
	}
	else {
		// Convert to RGBA
		cv::cvtColor(input, output, cv::COLOR_BGR2BGRA); // BGR to BGRA
	}

	return output;
}

void generator::alphaComposite(const cv::Mat& baseLayer, const cv::Mat& frontLayer, cv::Mat& res) {
	CV_Assert(baseLayer.size() == frontLayer.size()); 
	CV_Assert(baseLayer.type() == CV_8UC4 && frontLayer.type() == CV_8UC4); 
	res.create(baseLayer.size(), baseLayer.type()); 

	const uchar* basePtr = baseLayer.data;
	const uchar* frontPtr = frontLayer.data;
	uchar* resPtr = res.data;

	int numPixels = baseLayer.rows * baseLayer.cols;
	for (int i = 0; i < numPixels; i++) {
		float alpha_B = frontPtr[i * 4 + 3] / 255.0f;
		if(alpha_B == 0 && frontPtr[i * 4 + 2] == 0 && frontPtr[i * 4 + 1] == 0)
			continue;

		for (int c = 0; c < 3; c++) {
			resPtr[i * 4 + c] = static_cast<uchar>(alpha_B * frontPtr[i * 4 + c] + (1 - alpha_B) * basePtr[i * 4 + c]);
		}
		resPtr[i * 4 + 3] = 255;
	}
}

std::optional<Trait> generator::generate_first_random_trait [[nodiscard]] () {
	std::sort(_traitsDirectories.begin(), _traitsDirectories.end(), [](auto& dir1, auto& dir2) {
		return dir1.get_path() < dir2.get_path();
	});

	for (int i = 0; i < _traitsDirectories.size(); i++) {
		_traitsDirectories[i].setId(i);

		for (auto& trait : _traitsDirectories[i].getTraits()) {
			trait.setTraitFolderId(i);
		}
	}

	auto time = Trait::get_unix_time();

	std::srand(time);

	return _traitsDirectories[0].getTraits()[std::rand() % _traitsDirectories[0].getTraits().size()];
}
