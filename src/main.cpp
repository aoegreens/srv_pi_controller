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

#include "web/get/GetGPIO.cpp"
#include "web/get/SetGPIO.cpp"

using namespace aoe::hw;
using namespace aoe::controller;
using namespace restbed;
using namespace std;

int main(const int, const char**)
{
	bio::log::Engine* logger = new bio::log::LogToCout();

	sgPinController = new PinController();
	sgPinController->SetLogEngine(logger);

    auto gpioGet = make_shared< Resource >();
    gpioGet->set_path("/v1/gpio/get");
    gpioGet->set_method_handler("GET", GetGPIO);

    auto gpioSet = make_shared< Resource >();
    gpioSet->set_path("/v1/gpio/set");
    gpioSet->set_method_handler("GET", SetGPIO);

    auto settings = make_shared< Settings >();
    settings->set_port(80);
    settings->set_default_header("Connection", "close");

    Service service;
    service.publish(gpioGet);
    service.publish(gpioSet);
    service.start(settings);

    delete sgPinController;

    return EXIT_SUCCESS;
}
