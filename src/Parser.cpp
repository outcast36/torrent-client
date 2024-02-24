#include <string>
#include <iostream>
#include <filesystem>
#include <cstdint>
#include <vector>
#include <regex>
#include "Parser.h"
#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

using namespace BitTorrent;

//utility function to strip any non-numeric characters from a string
std::string stripAlpha(std::string input){
    std::string stripped="";
    for (char ch:input){
        if (isdigit(ch)) stripped+=ch;
    }
    return stripped;
}

Parser::Parser(std::string metainfo) : torrent(metainfo) {}

std::string Parser::getMetainfo() { 
    return this->torrent; 
}

bool Parser::isTorrentFile(std::string metainfo) {
    return std::filesystem::path(metainfo).extension() == ".torrent";
}

//json decodeItem(){
//    if token == 'i'
//    else if token == 'l'
//    else if token == 'd'
//    else if isdigit(token)
//    else 
//}

//convert a bencoded string into a list of tokens, for all types except strings, tokenized objects consist of a type code [idl], the object value, and an end code "e"
//strings are given a virtual type code "s" followed by the string data
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

//Parse byte strings encoded as <string length encoded in base ten ASCII>:<string data>
json Parser::decodeString(std::string encoded){
    //encoded value begins with length of string and contains the colon designating the start of the string
    size_t colonIndex = encoded.find(":");
    if (isdigit(encoded[0]) && colonIndex !=std::string::npos){
        std::string numberString = encoded.substr(0,colonIndex);
        int64_t length = stoll(numberString);
        std::string stringData = encoded.substr(colonIndex+1);
        //enforces that there must be exactly length bytes following the colon 
        //enforces that strings must be encoded as <[0-9]*>:<string data with n bytes where n == number preceding the colon>
        if ((stripAlpha(numberString).size() == numberString.size()) && (stringData.size()==length)) return json(stringData);
        throw std::runtime_error("Invalid encoded value: " + encoded); 
    }
    else throw std::runtime_error("Invalid encoded value: " + encoded);
}

//Parse ints encoded as i<integer encoded in base ten ASCII> e
std::int64_t Parser::decodeInt(std::string encoded){
    if (encoded.size() >=3 && encoded[0]=='i'){
        size_t eIndex = encoded.find("e");
        bool leadingZero = (encoded[1]=='0' && encoded[2]!='e'); //anything of the form "i0...e" other than "i0e" is invalid
        bool negativeLeadingZero = (encoded[1]=='-' && encoded[2]=='0'); //anything of the form "i-0...e" is invalid
        bool eInvalid = (eIndex==std::string::npos || eIndex!=encoded.size()-1); //e not found or e is not the last character of the string
        if (leadingZero || negativeLeadingZero || eInvalid) throw std::runtime_error("Invalid encoded value: " + encoded);
        else {
            std::string numberString = encoded.substr(1, encoded.size()-2);
            if (stripAlpha(numberString).size() != numberString.size()) throw std::runtime_error("Invalid encoded value: " + encoded);
            else return stoll(numberString);
        }
    }
    else throw std::runtime_error("Invalid encoded value: " + encoded);
}

//Parse bencoded lists where each element can be any other bencoded type. Construct a vector of encoded strings and decode as needed
std::vector<std::string> Parser::decodeList(std::string encoded){
    return {};
}