#include <filesystem>
#include <string>
#include "Parser.h"
#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

using namespace BitTorrent

bool isTorrentFile(std::string metainfoFile){
    return std::filesystem::path(metainfoFile).extension() == ".torrent";
}

int main(int argc, char *argv[]){
    return 0;
}