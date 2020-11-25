#include "Utils.h"

#include <fstream>
#include <vector>

//#include <cereal/archives/json.hpp>

/**
 * @brief Convert string to wString.
 * @param str string
 * @return wString
*/
std::wstring Utils::convertStringToWString(std::string str)
{
    return std::wstring(str.begin(), str.end());
}

/**
 * @brief Convert wStringToString
 * @param wstr wString
 * @return string
*/
std::string Utils::convertWStringToString(std::wstring wstr)
{
    return std::string(wstr.begin(), wstr.end());
}


std::vector<char> Utils::HexToBytes(const std::string& hex) {
    std::vector<char> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    return bytes;
}
