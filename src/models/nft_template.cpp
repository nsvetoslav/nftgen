#include "nft_template.hpp"

namespace nftgen {

    json Attribute::to_json() const {
        return json{{"trait_type", trait_type}, {"value", value}};
    }

    void Attribute::from_json(const json &j) {
        if (j.contains("trait_type") && j.contains("value")) {
            j.at("trait_type").get_to(trait_type);
            j.at("value").get_to(value);
        }
    }

    json File::to_json() const {
        return json{{"uri", uri}, {"type", type}};
    }

    void File::from_json(const json &j) {
        if (j.contains("uri") && j.contains("type")) {
            j.at("uri").get_to(uri);
            j.at("type").get_to(type);
        }
    }

    json Creator::to_json() const {
        return json{{"address", address}, {"share", share}};
    }

    void Creator::from_json(const json &j) {
        if (j.contains("address") && j.contains("share")) {
            j.at("address").get_to(address);
            j.at("share").get_to(share);
        }
    }

    json Collection::to_json() const {
        return json{{"name", name}, {"family", family}};
    }

    void Collection::from_json(const json &j) {
        if (j.contains("name") && j.contains("family")) {
            j.at("name").get_to(name);
            j.at("family").get_to(family);
        }
    }

    json NFT_Metadata::to_json() const {
        json attributesJson = json::array();
        for (const auto &attr : attributes) { attributesJson.push_back(attr.to_json()); }

        json filesJson = json::array();
        for (const auto &file : files) { filesJson.push_back(file.to_json()); }

        json creatorsJson = json::array();
        for (const auto &creator : creators) { creatorsJson.push_back(creator.to_json()); }

        return json{{"name", name},
                    {"symbol", symbol},
                    {"description", description},
                    {"seller_fee_basis_points", seller_fee_basis_points},
                    {"image", image},
                    {"external_url", external_url},
                    {"attributes", attributesJson},
                    {"collection", collection.to_json()},
                    {"properties", json{{"files", filesJson}, {"category", category}, {"creators", creatorsJson}}}};
    }

    void NFT_Metadata::from_json(const json &j) {
        if (j.contains("name") && j.contains("symbol") && j.contains("description") &&
            j.contains("seller_fee_basis_points") && j.contains("image") && j.contains("external_url") &&
            j.contains("attributes") && j.contains("collection") && j.contains("properties")) {
            j.at("name").get_to(name);
            j.at("symbol").get_to(symbol);
            j.at("description").get_to(description);
            j.at("seller_fee_basis_points").get_to(seller_fee_basis_points);
            j.at("image").get_to(image);
            j.at("external_url").get_to(external_url);

            if (j["attributes"].is_array()) {
                for (const auto &attr : j["attributes"]) {
                    Attribute attribute;
                    attribute.from_json(attr);
                    attributes.push_back(attribute);
                }
            }

            collection.from_json(j.at("collection"));

            if (j["properties"].contains("files") && j["properties"]["files"].is_array()) {
                for (const auto &file : j["properties"]["files"]) {
                    File fileObj;
                    fileObj.from_json(file);
                    files.push_back(fileObj);
                }
            }
            j.at("properties").at("category").get_to(category);

            if (j["properties"].contains("creators") && j["properties"]["creators"].is_array()) {
                for (const auto &creator : j["properties"]["creators"]) {
                    Creator creatorObj;
                    creatorObj.from_json(creator);
                    creators.push_back(creatorObj);
                }
            }
        }
    }
} // namespace nftgen