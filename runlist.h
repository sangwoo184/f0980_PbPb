// runlist.h - RapidJSON 기반 runlist.json 로드 (고정된 key 사용)
#pragma once

#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <iostream>
#include <string>

namespace runlist {

inline rapidjson::Value& load(rapidjson::Document& doc, const std::string& filename = "runlist.json") {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "[runlist.h] Failed to open " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }

    rapidjson::IStreamWrapper isw(ifs);
    doc.ParseStream(isw);

    if (doc.HasParseError()) {
        std::cerr << "[runlist.h] Failed to parse JSON" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const char* key = "singlerun"; // 원하는 키를 고정

    if (!doc.HasMember(key)) {
        std::cerr << "[runlist.h] Key \"" << key << "\" not found in JSON" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return doc[key];
}

} // namespace runlist

// auto& Runs = doc["runs"];
// auto& Runs = doc["runWoInitial"];
// auto& Runs = doc["rotationalrun"];
// auto& Runs = doc["local"];
// auto& Runs = doc["kiaf"];