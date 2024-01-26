#include <string>
#include <cstdint>
#include <unordered_map>

namespace BitTorrent {
    class Parser {
    public:
        Parser(std::string metainfo);
        std::string getMetainfo();
        bool isTorrentFile(std::string metainfo);
        std::int64_t decodeInt(std::string encoded);
        std::string decodeString(std::string encoded);
    private:
        std::string torrent;
    };
}


