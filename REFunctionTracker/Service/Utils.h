#pragma once
#include <string>
#include <wstring.h>
using namespace std;

class Utils
{
public:
	static wstring convertStringToWString(string str);
	static string convertWStringToString(wstring str);
};

