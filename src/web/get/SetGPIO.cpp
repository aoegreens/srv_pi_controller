#include "web/get/SetGPIO.h"

namespace aoe {
namespace controller {

void SetGPIO(const std::shared_ptr< restbed::Session > session)
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

	const std::vector< RequiredParameter > requiredParameters = {{
		"pin",
		"pin",
		{},
		""
	}, //the wiringPi pin to use.
		{
			"state",
			"state",
			{},
			""
		}, //the state to set; "ON", "OFF". '"TOGGLE'.
	};
	for (
		const auto& req: requiredParameters
		)
	{
		if (!request->has_query_parameter(req.name))
		{
			if (req.defaultVal.empty())
			{
				session->close(
					400,
					"Please specify \"" + req.name + "\""
				);
				return;
			}
		}
	}

	std::string stateString = request->get_query_parameter("state");
	hw::GPIOState toSet = hw::gpio_state::Toggle();
	if (stateString == "ON")
	{
		toSet = hw::gpio_state::On();
	}
	else if (stateString == "OFF")
	{
		toSet = hw::gpio_state::Off();
	}

	std::string pinString = request->get_query_parameter("pin");
	hw::Pin pin = std::stoi(pinString);

	sgPinController->SetGPIOState(
		pin,
		toSet
	);

	session->close(
		restbed::OK,
		"complete."
	);
}

} //controller namespace
} //aoe namespace
