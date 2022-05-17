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
