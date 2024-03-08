#include <string>
#include <cstdint>
#include <vector>
#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

namespace BitTorrent {
    class Parser {
    public:
        Parser(std::string metainfo);
        std::string getMetainfo();
        bool isTorrentFile(std::string metainfo);
        std::vector<std::string> tokenize(std::string encoded);
        json decode(std::vector<std::string> tokens, int& idx);
    private:
        std::string torrent;
    };
}


