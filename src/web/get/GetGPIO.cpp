#include "web/get/GetGPIO.h"

namespace aoe {
namespace controller {

void GetGPIO(const std::shared_ptr< restbed::Session > session)
{
	auto request = session->get_request();

	#if 1
	std::string parameters;
	for (
		const auto& param: request->get_query_parameters())
	{
		parameters += "{" + param.first + " : " + param.second + "} ";
	}
	fprintf(
		stdout,
		"Processing gpio request for: {\n    %s\n}\n",
		parameters.c_str());
	#endif

	if (!request->has_query_parameter("pin"))
	{
		session->close(
			400,
			"You must specify a pin."
		);
		return;
	}

	std::stringstream ret;
	hw::Pin pin = std::stoi(request->get_query_parameter("pin"));

	if (sgPinController->GetGPIODirection(pin) == hw::gpio_direction::In())
	{
		ret << sgPinController->ReadGPIOInput(pin);
	}
	else
	{
		ret << hw::GPIOStatePerspective::Instance().GetNameFromId(sgPinController->GetGPIOState(pin));
	}

	session->close(
		restbed::OK,
		ret.str());
}

} //controller namespace
} //aoe namespace
