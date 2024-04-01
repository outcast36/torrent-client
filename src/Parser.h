#include <string>
#include <cstdint>
#include <vector>
#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

namespace BitTorrent {
    class Parser {
    public:
        Parser(std::string metainfo);
        void decodeFile(json& decoded, std::string& info);
    private:
        std::string torrent;
        std::vector<std::string> tokenize(std::string encoded);
        json decodeTokens(std::vector<std::string>& tokens, int64_t& idx);
    };
}


