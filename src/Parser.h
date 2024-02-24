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
        json decodeString(std::string encoded);
        std::int64_t decodeInt(std::string encoded);
        std::vector<std::string> decodeList(std::string encoded);
        // map<string,string> decodeDict(std::string encoded);
    private:
        std::string torrent;
    };
}


