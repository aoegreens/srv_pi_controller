// This file is a part of the AOE Greens code base.
// AOE Greens has provided this code to you in the hopes of
// making sustainable food production a world-wide reality.
// For more information on our open source software and how to reach us,
// please visit https://aoegreens.com/about.
//
// Copyright (C) 2022 AOE GREENS LLC and its founders:
// Taylor Parrish and Séon O'Shannon
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>

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
