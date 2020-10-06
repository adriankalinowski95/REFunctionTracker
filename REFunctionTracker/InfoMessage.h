#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <string>

struct InfoMessageData {
	std::string message;
	int status;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("message", message)
			, cereal::make_nvp("status", status));
	}
};

class InfoMessage {

public:
enum Status {
		S_ERROR = 1,
		S_SUCCESS = 2,
		S_UNKOWN = 4
	};

public:
	InfoMessage(std::string message, int status);
	std::string toJson();

private:
	std::string message;
	int status;
};