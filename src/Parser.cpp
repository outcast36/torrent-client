#include <string>
#include <iostream>
#include <filesystem>
#include <cstdint>
#include <vector>
#include <regex>
#include "Parser.h"
#include "lib/nlohmann/json.hpp"

using json = nlohmann::json; //namespace alias

using namespace BitTorrent;

Parser::Parser(std::string metainfo) : torrent(metainfo) {}

std::string Parser::getMetainfo() { 
    return this->torrent; 
}

bool Parser::isTorrentFile(std::string metainfo) {
    return std::filesystem::path(metainfo).extension() == ".torrent";
}

//convert a bencoded string into a list of tokens, for all types except strings, tokenized objects consist of a type code [idl], the object value, and an end code "e"
//strings are given a virtual type code "s" followed by the string data
//based off of https://web.archive.org/web/20200105114449/https://effbot.org/zone/bencode.htm
// "i475e" -> ["i", "475", "e"] -- int
// "l4:spam4:eggse" -> ["l", "s", "spam", "s", "eggs", "e"] -- list
// "d3:cow3:moo4:spam4:eggse" -> ["d", "s", "cow", "s", "moo", "s", "spam", "s", "eggs", "e"] -- dict
// "5:hello" -> ["s", "hello"] -- string
// "li475el4:spam3:and4:eggsee" -> ["l", "i", "475", "e", "l", "s", "spam", "s", "and", "s", "eggs", "e", "e"]
std::vector<std::string> Parser::tokenize(std::string encoded){
    const std::regex pattern("([idel])|(\\d+):|(-?\\d+)");
    std::vector<std::string> tokens;
    std::smatch results;
    int64_t idx=0;
    while (idx<encoded.size()){
        std::string sub = encoded.substr(idx, encoded.size()-idx);
        if (std::regex_search(sub, results, pattern)){
            int64_t tokLen = std::distance(results[0].first, results[0].second); // distance from the start of the matching sequence to the end of the matching sequence
            std::string s = encoded.substr(idx, tokLen);
            idx+=tokLen;
            if (results[2].matched){ // capturing groups are 1-indexed, string matching group is index 2
                tokens.push_back("s");
                int64_t strLen = std::stoll(s);
                tokens.push_back(encoded.substr(idx, strLen));
                idx+=strLen;
            }
            else tokens.push_back(results.str());
        }
        else throw std::runtime_error("Invalid encoded value: " + encoded);
    }
    return tokens;
}

//Given a list of tokens for a bencoded torrent file, decode it into a json object representing the decoded dict
json Parser::decode(std::vector<std::string> tokens, int& idx){
    if (tokens[idx]=="i"){
        std::string data = tokens[++idx];
        if (tokens[++idx]!="e") throw std::runtime_error("Expected 'e' token i<>e: " + tokens[idx]); 
        idx++;
        return json(std::stoll(data));
    }
    else if (tokens[idx]=="s"){
        std::string data = tokens[++idx];
        idx++;
        return json(data);
    }
    else if (tokens[idx]=="l"){
        std::vector<json> items;
        idx++; 
        while (tokens[idx]!="e"){
            json item = decode(tokens,idx);
            items.push_back(item);
        }
        return json(items);
    }
    else if (tokens[idx]=="d"){
        idx++;
        json j_dict;
        while (tokens[idx]!="e"){
            if (tokens[idx++]=="s"){
                std::string key = tokens[idx++];
                json value = decode(tokens, idx);
                j_dict[key]=value;
            }
            else throw std::runtime_error("Expected string key: " + tokens[idx]);
        }
        return j_dict;
    }
    else throw std::runtime_error("Invalid token sequence");
}