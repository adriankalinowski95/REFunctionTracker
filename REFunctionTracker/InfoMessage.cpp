#include "InfoMessage.h"
#include "Utils.h"

/* https://www.w3schools.com/howto/howto_js_alert.asp */
InfoMessage::InfoMessage(std::string message, int status):
	message(message),status(status)
{
}

std::string InfoMessage::toJson()
{
	InfoMessageData infoMessData { this->message,this->status };
	return Utils::serializeToJSON<InfoMessageData>(infoMessData,"info-message");
}
