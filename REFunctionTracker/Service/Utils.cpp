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

