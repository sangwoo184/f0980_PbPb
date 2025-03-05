// const std::string jsonName = "local";
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json runlist() {
    std::ifstream file("runlist.json");
    json inputJson = json::parse(file);
    auto Runs = inputJson["singlerun"];
    // auto Runs = inputJson["runs"];
    // auto Runs = inputJson["runWoInitial"];
    // auto Runs = inputJson["rotationalrun"];
    // auto Runs = inputJson["local"];
    // auto Runs = inputJson["kiaf"];
    return Runs;
}