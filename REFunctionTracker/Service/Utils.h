#pragma once
#include <string>
#include <wstring.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>

class Utils
{
public:
	static std::wstring convertStringToWString(std::string str);
	static std::string convertWStringToString(std::wstring str);

public:
    template<typename T> static std::string serializeToJSON(T tObject, std::string paramName);
    template<typename T> static T serializeToObject(std::string json);
};

/**
 * @brief Convert object to string JSON.
 * @tparam T Type
 * @param tObject Object<T>
 * @param paramName Param name in string.
 * @return std::string
*/
template<typename T>
std::string Utils::serializeToJSON(T tObject,std::string paramName)
{
    std::stringstream ss;
    cereal::JSONOutputArchive archive_out(ss);
	archive_out(cereal::make_nvp(paramName.c_str(), tObject));
    return std::string(ss.str()+"\r\n}");
}

/**
 * @brief Convert json to object
 * @tparam T Type
 * @param json JSON
 * @return Object<T>
*/
template<typename T>
T Utils::serializeToObject(std::string json)
{
    T tObject;
    std::stringstream ss(json);
    cereal::JSONInputArchive archive_in(ss);
    archive_in(tObject);
    return tObject;
}





/* Cereal example! serialize
* 	std::string typicalString;
	ProcData data {std::string("damian"),123 };
	ProcData dat2a { std::string("damia2n"), 123 };

	std::vector<ProcData> vectr;
	vectr.push_back(data);
	vectr.push_back(dat2a);

	std::stringstream ss;
	cereal::JSONOutputArchive archive(ss);
	archive(vectr);

*/


/*Cereal example! deserialize
*
* 
	std::string cc= R"({
		"vectr": [
	{
		"processName": "damian",
			"processPID" : 123
	},
		{
			"processName": "damia2n",
			"processPID" : 123
		}
		]} )";

	std::stringstream ss(cc);
	std::vector<ProcData> newV;
	cereal::JSONInputArchive archive_in(ss);
	archive_in(newV);
*/
