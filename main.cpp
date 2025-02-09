#include "src/workflow/logger.hpp"
#include "src/workflow/settings.hpp"
#include "src/workflow/generator.hpp"

 int main()
{
    if (!nftgen::settings::GetInstance().load())
        return -1;

    nftgen::generator generator(nftgen::settings::GetInstance().GetAssetsDirectory());

    const auto& generation_result = generator.generate(1000);

    return 1;
}

//
//#include <iostream>
//#include "src/database/db_global.h"
//
//namespace nftgen
//{
//namespace Tables
//{
//struct GeneratedNfts
//{
//    struct _alias_t
//    {
//        static constexpr const char _literal[] = "GeneratedNfts";
//    };
//
//    struct Id
//    {
//        struct _alias_t
//        {
//            static constexpr const char _literal[] = "id";
//        };
//
//        using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
//    };
//
//    struct MetadataUuid
//    {
//        struct _alias_t
//        {
//            static constexpr const char _literal[] = "metadata_uuid";
//        };
//
//        using _traits = sqlpp::make_traits<sqlpp::text>;
//    };
//
//    struct DirectoryId
//    {
//        struct _alias_t
//        {
//            static constexpr const char _literal[] = "directory_id";
//        };
//
//        using _traits = sqlpp::make_traits<sqlpp::bigint>;
//    };
//
//    struct Directory
//    {
//        struct _alias_t
//        {
//            static constexpr const char _literal[] = "directory";
//        };
//
//        using _traits = sqlpp::make_traits<sqlpp::text>;
//    };
//
//    struct TraitId
//    {
//        struct _alias_t
//        {
//            static constexpr const char _literal[] = "trait_id";
//        };
//
//        using _traits = sqlpp::make_traits<sqlpp::bigint>;
//    };
//
//    struct TraitName
//    {
//        struct _alias_t
//        {
//            static constexpr const char _literal[] = "trait_name";
//        };
//
//        using _traits = sqlpp::make_traits<sqlpp::text>;
//    };
//
//    using _columns = sqlpp::detail::type_vector<Id, MetadataUuid, DirectoryId, Directory, TraitId, TraitName>;
//};
//}  // namespace Tables
//}  // namespace nftgen
//
//class GeneratedNftsManager
//{
//public:
//    explicit GeneratedNftsManager(const std::shared_ptr<sqlpp::postgresql::connection_config> &config) :
//        db(config)
//    {
//    }
//
//    void insertDemoRecord()
//    {
//        nftgen::Tables::GeneratedNfts table;
//
//        sqlpp::text uuid;
//        char uuid_str[37];
//
//        db(sqlpp::postgresql::insert_into(table));
//
//        std::cout << "Inserted demo record successfully!" << std::endl;
//    }
//
//    // void fetchAllRecords()
//    //{
//    //     nftgen::Tables::GeneratedNfts table;
//    //     for (const auto &row : db(select(all_of(table)).from(table).unconditionally()))
//    //     {
//    //         std::cout << "ID: " << row.id << ", UUID: " << row.metadata_uuid << ", Directory: " << row.directory << ", Trait Name: " << row.trait_name << std::endl;
//    //     }
//    // }
//
//private:
//    sqlpp::postgresql::connection db;
//};
//
//int main()
//{
//    // Initialize the global connection variable
//    auto config = std::make_shared<sqlpp::postgresql::connection_config>();
//    config->dbname = "HackjaggoNFTGen";
//    config->user = "postgres";
//    config->password = "1122395sS#";
//    config->host = "localhost";
//    config->debug = false;
//    db_global_init(config);
//
//    try
//    {
//        GeneratedNftsManager manager(config);
//        manager.insertDemoRecord();
//    }
//    catch (const std::exception &e)
//    {
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//
//    return 0;
//}
