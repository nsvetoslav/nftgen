#include "generator.hpp"

#include <filesystem>
#include <format>

#include "generator.hpp"
#include "settings.hpp"
#include "../exceptions/factories.hpp"
#include "../exceptions/exception.hpp"
#include <memory>

#define ApplyFromDirectory(stringVal) stringVal
#define ApplyToTrait(stringVal) stringVal
#define ApplyFromTrait(stringVal) stringVal
#define ApplyFromTraitDirectory(stringVal) stringVal
#define ApplyToDirectory(stringVal) stringVal
#define NoTraitApply ""
#define GenerateOnlyWithTraits(val) val

using namespace nftgen;

std::vector<TraitDirectory> generator::_traits_directories{};

[[nodiscard]] bool generator::load_directories()
{
    std::string root_path = _traits_directories_root_path;
    utilities::filemanager fileManager;

    if (!std::filesystem::exists(root_path) && !std::filesystem::is_directory(root_path))
    {
        // logger.log(!fileManager.exists(), _traitsDirectory);
        return false;
    }

    // Traversing directories in assets
    std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> trait_directory_paths_pairs;
    fileManager.get_file_paths_in_directory(root_path, trait_directory_paths_pairs);

    for (int trait_directory_path_index = 0; trait_directory_path_index < trait_directory_paths_pairs.size(); ++trait_directory_path_index)
    {
        auto &trait_directory_path_pair = trait_directory_paths_pairs[trait_directory_path_index];
        const auto &trait_directory_path = trait_directory_path_pair.first;
        const auto &trait_directory_folder_name = trait_directory_path_pair.second;

        // set traitFoldersExceptions
        //

        TraitDirectory trait_directory;
        trait_directory.set_path(trait_directory_path);
        trait_directory.set_directory_name(trait_directory_folder_name);
        trait_directory.set_id(trait_directory_path_index);

        if (trait_directory_folder_name == "6.Earings")
        {
            trait_directory.set_generation_chance(0.05);
        }
        if (trait_directory_folder_name == "9.Extras")
        {
            trait_directory.set_generation_chance(0.08);
        }
        if (trait_directory_folder_name == "Legacy")
        {
            trait_directory.set_generation_chance(0.03);
        }
        if (trait_directory_folder_name == "8.Top Acc")
        {
            trait_directory.set_generation_chance(0.5);
        }

        // All the trait files paths in a traitDirectory
        std::vector<std::pair<std::string /*path*/, std::string> /*fileName*/> traits_paths;
        fileManager.get_file_paths_in_directory(trait_directory_path_pair.first, traits_paths);

        // loadTraitsExceptions
        //
        trait_directory.set_traits(traits_paths, trait_directory_folder_name, trait_directory_path_index, trait_directory_path_index);

        // mem moving traitFolder in _traitsDirectories
        _traits_directories.push_back(trait_directory);
    }

    nftgen::calculator::set_full_generation_chances(_traits_directories);

    // Assign generation order based on sorted positions
    for (size_t i = 0; i < _traits_directories.size(); ++i)
    {
        for (auto &trait : _traits_directories.at(i).get_traits())
            trait.set_gen_order_trait_directory_id(i);

        _traits_directories[i].set_gen_order(static_cast<int>(i));
    }

    // Sort by traits size
    std::sort(_traits_directories.begin(), _traits_directories.end(), [](TraitDirectory &dir1, TraitDirectory &dir2) {
        if (dir1.get_generation_chance() != dir2.get_generation_chance())
        {
            return dir1.get_generation_chance() < dir2.get_generation_chance();
        }
        return dir1.get_traits().size() > dir2.get_traits().size();
    });

    return true;
}

void generator::create_gen_directory(std::string_view dir) const
{
    if (std::filesystem::exists(dir))
        return;

    std::filesystem::create_directory(dir);
}

[[nodiscard]] bool nftgen::generator::add_generated_nft(nftgen::NFT_Metadata &nft_metadata)
{
    size_t traits_hash = nft_metadata.generate_traits_hash();
    if (_nfts_hashes.find(traits_hash) != _nfts_hashes.end())
    {
        return false;
    }

    _nfts_hashes.insert(traits_hash);
    _generated_nfts_metadatas.push_back(nft_metadata);

    for (const auto &trait : nft_metadata.get_traits())
    {
        _nfts_grouped_traits[std::make_pair(trait.get_gen_order_trait_directory_id(), trait.get_trait_id())]++;
    }

    return true;
}

nftgen::generator::generator(std::string trait_directories_root_path) :
    _traits_directories_root_path(trait_directories_root_path)
{
    _generated_nfts_directory = nftgen::settings::get_instance().get_generated_nfts_directory();
    create_gen_directory(_generated_nfts_directory);
}

[[nodiscard]] bool generator::generate(unsigned long nfts_count)
{
    try
    {
        json j;
        std::ifstream nft_metadata_template_file(settings::get_instance().get_template_file());
        nft_metadata_template_file >> j;
        _nft_template_metadata.from_json(j);

        if (!nft_metadata_template_file.is_open())
        {
            std::cerr << "Could not open the templateNFTMetadata file!" << std::endl;
            return false;
        }

        // json exceptions_json;
        // std::string exceptions_file_path = settings::get_instance().get_working_directory() + "\\exceptions.json";
        // std::ifstream nft_exceptions_json_file(exceptions_file_path);
        // if (!nft_exceptions_json_file.is_open())
        //{
        //     std::cerr << "Could not open the templateNFTMetadata file!" << std::endl;
        //     return false;
        // }

        // nft_exceptions_json_file >> exceptions_json;

        if (!load_directories())
            return false;

        if (_traits_directories.empty())
            return false;

        // -If "Hood" is generated you cannot generate "Horns" or "Top Acc".
        _exceptions.traitDirectoriesExceptions.push_back(
            TraitDirectoryException(
                ApplyFromDirectory("Top Acc"),
                ApplyToDirectory("Horns"),
                ExceptionsTypes::NotGenerateDirectoryWithDirectory));

        // -If "Rainbow Mouth" is generated it is generated at the end thus no "Extras" are generated after.
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Hood"),
            ApplyFromTraitDirectory("Legacy"),
            NoTraitApply,  // --
            ApplyToDirectory("Horns"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Hood"),
            ApplyFromTraitDirectory("Legacy"),
            NoTraitApply,  // --
            ApplyToDirectory("Top Acc"),
            ExceptionsTypes::NotGenerateTraitWithDirectory,
            std::nullopt));

        // - If "Beard" is generated it is placed in the 3rd position right before "Eyes" and "Mouths".
        // -- For last generation

        // - If any variation of the "Cap" Trait is generated no "Earings" or "Clothes" with back pieces can be generated.
        _exceptions.traitsExceptions.push_back(TraitException(ApplyFromTrait("Cap"),
                                                              ApplyFromDirectory("Top Acc"),
                                                              NoTraitApply,
                                                              ApplyToDirectory("Earings"),
                                                              ExceptionsTypes::NotGenerateTraitWithDirectory,
                                                              std::nullopt));

        // For last generation type
        //_exceptions.traitsExceptions.push_back(TraitException(ApplyFromTrait("Cap"),
        //                                                      ApplyFromDirectory("Top Acc"),
        //                                                      NoTraitApply,
        //                                                      ApplyToDirectory("Clothes"),
        //                                                      ExceptionsTypes::NotGenerateTraitWithDirectory,
        //                                                      std::nullopt));

        // - "Beanie" trait should only be generated with simple clothing (Stripped Shirt, Tank Top, Hoodie. Tee,)
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Beanie"),
            ApplyFromDirectory("Top Acc"),
            NoTraitApply,  // --
            ApplyToDirectory("Clothes"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Stripped Shirt",
                "Tanktop",
                "Hoodie",
                "Tee"}));  // generate only with those traits

        // - Gold Horns are only generated with Gold Skin
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Gold Horns"),
            ApplyFromDirectory("Horns"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Gold Base"}));  // generate only with those traits

        // - Silver Horns are only generated with Silver Skin
        _exceptions.traitsExceptions.push_back(TraitException(
            ApplyFromTrait("Silver Horns"),
            ApplyFromDirectory("Horns"),
            NoTraitApply,  // --
            ApplyToDirectory("Base"),
            ExceptionsTypes::GenerateTraitOnlyWithTraits,
            std::vector<std::string>{
                "Silver Base"}));  // generate only with those traits

        // - "Flame Horns" should be generated only with darker backgrounds
        // - Any type of "Wood Horns" should not be generated with "Brown Skin"
        // - "Rainbow Mouth" should not be genereted with "Old Time Slot Machine"
        // - "Red Skin" should not be genereted with "Red Kimono"
        // - "Blush Mouth" Should not be genereted with "Silver Skin"
        // - "Anime Eyes" should not be genereted with "Tie" and "Headband" traits
        // - "Anime Eyes" Should be generated with the same color skin
        // - "Golden Glasses" should be generated more with "Scar Eye"
        // - "Beanie" trait should only be generated with simple clothing (Stripped Shirt, Tank Top, Hoodie. Tee,)

        // - If "Hood" is generated you cannot generate "Horns" or "Top Acc".
        // - If "Rainbow Mouth" is generated it is generated at the end thus no "Extras" are generated after.
        // - If "Beard" is generated it is placed in the 3rd position right before "Eyes" and "Mouths".
        // - If "Top Acc" is generated no "Horns" are generated.
        // - If any variation of the "Cap" Trait is generated no "Earings" or "Clothes" with back pieces can be generated.
        // - Extras should be rarely generated on some of the NFT's (the lowest rate of generated traits

        // TraitDirectoryException

        int generated_nfts_count = 0;
        while (generated_nfts_count < nfts_count)
        {
            std::cout << "Starting " << generated_nfts_count << "th nft generation." << std::endl;
            generate_single_nft(generated_nfts_count);
        }

        //// Sort by traits size
        // std::sort(_traits_directories.begin(), _traits_directories.end(), [](TraitDirectory &dir1, TraitDirectory &dir2) {
        //     if (dir1.get_generation_chance() != dir2.get_generation_chance())
        //     {
        //         return dir1.get_generation_chance() < dir2.get_generation_chance();
        //     }
        //     return dir1.get_traits().size() > dir2.get_traits().size();
        // });

        save_nfts_in_parallel(_generated_nfts_metadatas);

        // std::vector<std::pair<std::pair<int, int>, size_t>> sorted_traits(_nfts_grouped_traits.begin(), _nfts_grouped_traits.end());

        //// Step 2: Sort the vector by traitFolderID
        // std::sort(sorted_traits.begin(), sorted_traits.end(), [](const auto &a, const auto &b) {
        //     return a.first.first < b.first.first;  // Sort by traitFolderID
        // });

        //// Output header
        // std::cout << "TraitFolderID | TraitID | Count\n";
        // std::cout << "------------------------------------\n";

        //// Step 3: Output the sorted traits
        // for (const auto &entry : sorted_traits)
        //{
        //     int trait_directory_id = entry.first.first;  // Folder ID
        //     int trait_id = entry.first.second;           // Trait ID
        //     size_t count = entry.second;                 // Count of times this trait was generated

        //    std::cout << trait_directory_id << "            | " << trait_id << "      | " << count << '\n';
        //}
    }
    catch (const std::exception &exception)
    {
        // logger.log(exception);
        return false;
    }

    return true;
}

void nftgen::generator::generate_single_nft(int &generated_nft_count)
{
    NFT_Metadata nft_metadata(_nft_template_metadata);

    Trait *current_trait = nullptr;

    auto trait_directory_index = 0;
    bool doNotApplyGenerationChances = false;
    while (trait_directory_index < _traits_directories.size())
    {
        // get_next_trait applies the generation chances, so only exclusion exceptions remain
        auto &&generatedPair = Trait::get_next_trait(trait_directory_index, doNotApplyGenerationChances);
        if (generatedPair.second == SkippedFolder)
        {
            trait_directory_index++;
            continue;
        }

        doNotApplyGenerationChances = false;

        // Something unexpected happened, trying again.
        current_trait = generatedPair.first;
        if (current_trait == nullptr)
            continue;

        bool bContinueNextDir = false;
        bool bContinueNextTrait = false;

        // On successfully generations
        if (generatedPair.second == SuccessfullyGenerated)
        {
            std::string_view generated_trait_dir_name = current_trait->get_directory_name();
            std::string_view generated_trait_name = current_trait->get_filename();

            const auto it = std::find_if(_exceptions.traitDirectoriesExceptions.begin(),
                                         _exceptions.traitDirectoriesExceptions.end(),
                                         [&nft_metadata, /*generated_trait_dir_name, metadata_trait, */
                                          &bContinueNextDir,
                                          &bContinueNextTrait,
                                          &current_trait](const TraitDirectoryException &traitDirException) {
                                             std::unique_ptr<nftgen::typedexceptions::IExceptionProcessor> exception_processor =
                                                 nftgen::typedexceptions::TraitDirectoriesExceptionsFactory::Get(nft_metadata, traitDirException, *current_trait);

                                             if (exception_processor.get() == nullptr)
                                                 return false;

                                             if (!exception_processor->MeetsGenerationConditions())
                                             {
                                                 bContinueNextDir = exception_processor->ContinueToNextDirectory();
                                                 bContinueNextTrait = exception_processor->ContinueToNextTrait();

                                                 return true;
                                             }

                                             return false;

                                             return false;
                                         });

            if (it != _exceptions.traitDirectoriesExceptions.end())
            {
                if (bContinueNextDir)
                {
                    trait_directory_index++;
                    continue;
                }

                if (bContinueNextTrait)
                {
                    doNotApplyGenerationChances = true;
                    continue;
                }
            }
        }

        const auto it2 = std::find_if(_exceptions.traitsExceptions.begin(),
                                      _exceptions.traitsExceptions.end(),
                                      [&nft_metadata, /*generated_trait_dir_name, metadata_trait, */ &bContinueNextDir, &bContinueNextTrait, &current_trait](
                                          const TraitException &traitException) {
                                          // auto &dir = _traits_directories.at(metadata_trait.get_gen_order_trait_directory_id());

                                          std::unique_ptr<nftgen::typedexceptions::IExceptionProcessor> exception_processor =
                                              nftgen::typedexceptions::TraitExceptionsFactory::Get(nft_metadata, traitException, *current_trait);

                                          if (exception_processor.get() == nullptr)
                                              return false;

                                          if (!exception_processor->MeetsGenerationConditions())
                                          {
                                              bContinueNextDir = exception_processor->ContinueToNextDirectory();
                                              bContinueNextTrait = exception_processor->ContinueToNextTrait();

                                              return true;
                                          }

                                          return false;
                                      });

        if (it2 != _exceptions.traitsExceptions.end())
        {
            if (bContinueNextDir)
            {
                trait_directory_index++;
                continue;
            }

            if (bContinueNextTrait)
            {
                doNotApplyGenerationChances = true;
                continue;
            }
        }

        // Continue to next folder.
        trait_directory_index++;
        nft_metadata.add_trait(*current_trait);
        continue;
        //}
    }

    // if the generated nft hasn't been generated before, we increase the
    // generated nfts count
    if (add_generated_nft(nft_metadata))
    {
        generated_nft_count++;
    }
    else
    {
        std::cout << "Skipping already generated nft with hash:" + std::to_string(nft_metadata.generate_traits_hash()) << std::endl;
    }
}

void nftgen::generator::set_generation_chances(TraitDirectory &trait_directory)
{
    nftgen::calculator calculator;
    calculator.set_equal_generation_chances(trait_directory.get_traits());
}

[[nodiscard]] cv::Mat generator::convert_to_rgba(const cv::Mat &input)
{
    cv::Mat output;

    if (input.channels() == 4)
    {
        // If already RGBA, just return a copy
        output = input.clone();
    }
    else
    {
        // Convert to RGBA
        cv::cvtColor(input, output, cv::COLOR_BGR2BGRA);  // BGR to BGRA
    }

    return output;
}

void generator::alpha_composite(const cv::Mat &base_layer, const cv::Mat &front_layer, cv::Mat &res)
{
    CV_Assert(base_layer.size() == front_layer.size());
    CV_Assert(base_layer.type() == CV_8UC4 && front_layer.type() == CV_8UC4);
    res.create(base_layer.size(), base_layer.type());

    const uchar *base_ptr = base_layer.data;
    const uchar *front_ptr = front_layer.data;
    uchar *result_ptr = res.data;

    int pixels_number = base_layer.rows * base_layer.cols;
    for (int i = 0; i < pixels_number; i++)
    {
        float alpha_B = front_ptr[i * 4 + 3] / 255.0f;
        if (alpha_B == 0 && front_ptr[i * 4 + 2] == 0 && front_ptr[i * 4 + 1] == 0)
            continue;

        for (int c = 0; c < 3; c++)
        {
            result_ptr[i * 4 + c] = static_cast<uchar>(alpha_B * front_ptr[i * 4 + c] + (1 - alpha_B) * base_ptr[i * 4 + c]);
        }

        result_ptr[i * 4 + 3] = 255;
    }
}

void generator::process_nfts(int start_index, int end_index, std::vector<NFT_Metadata> &_generated_nfts)
{
    for (int nft_index = start_index; nft_index < end_index; ++nft_index)
    {
        auto &generated_nft = _generated_nfts[nft_index];
        generated_nft.sort_traits();

        const auto &traits = generated_nft.get_traits();

        cv::Mat base_layer = traits.front().get_matrix();
        base_layer = convert_to_rgba(base_layer);
        cv::Mat res(base_layer.size(), base_layer.type());

        for (int trait_index = 0; trait_index < generated_nft.get_traits().size(); ++trait_index)
        {
            const auto &current_trait = traits[trait_index];

            cv::Mat front_layer = current_trait.get_matrix();
            if (front_layer.empty())
            {
                std::cerr << "Error loading front layer image: " << current_trait.get_filename() << std::endl;
                continue;
            }

            front_layer = convert_to_rgba(front_layer);
            alpha_composite(base_layer, front_layer, res);
            base_layer = res.clone();
        }

        const std::string generated_nft_number = std::to_string(nft_index + 1);
        // Save the generated NFT
        cv::imwrite(_generated_nfts_directory + "/" + generated_nft_number + ".png", res, {cv::ImwriteFlags::IMWRITE_PNG_COMPRESSION, 1});
        generated_nft.name += " #" + generated_nft_number;
        // Save the NFT metadata
        std::ofstream output_file(_generated_nfts_directory + "/" + generated_nft_number + ".json", std::ios::trunc);
        if (output_file.is_open())
        {
            output_file << generated_nft.to_json().dump(4);
            output_file.close();
        }
        else
        {
            std::cout << "Failed to save nft " + generated_nft_number + " metadata" << std::endl;
        }
    }
}

void generator::save_nfts_in_parallel(std::vector<NFT_Metadata> &_generated_nfts)
{
    const int threads_count = 6;
    const int chunk_size = _generated_nfts.size() / threads_count;
    std::vector<std::thread> threads;

    // Create threads
    for (int i = 0; i < threads_count; ++i)
    {
        int start_index = i * chunk_size;
        int end_index = (i == threads_count - 1) ? _generated_nfts.size() : (i + 1) * chunk_size;

        // Launch a new thread for each chunk
        threads.push_back(std::thread(&generator::process_nfts, this, start_index, end_index, std::ref(_generated_nfts)));
    }

    // Join threads back to the main thread
    for (auto &t : threads)
    {
        t.join();
    }
}
