#include "Utils.h"

wstring Utils::convertStringToWString(string str)
{
    return wstring(str.begin(), str.end());
}

string Utils::convertWStringToString(wstring wstr)
{
    return string(wstr.begin(), wstr.end());
}
