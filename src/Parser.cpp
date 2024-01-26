#include <string>
#include <iostream>
#include <filesystem>
#include <cstdint>
#include "Parser.h"

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

//What to do if string has i..e but has non numeric characters in between i and e
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

std::string Parser::decodeString(std::string encoded){
    //encoded value begins with length of string and contains the colon designating the start of the string
    size_t colonIndex = encoded.find(":");
    if (isdigit(encoded[0]) && colonIndex !=std::string::npos){ 
        int64_t length = stoll(encoded.substr(0,colonIndex));
        return encoded.substr(colonIndex+1, length);
    }
    else throw std::runtime_error("Invalid encoded value: " + encoded);
}
