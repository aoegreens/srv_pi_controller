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

#pragma once

#include "common/Externals.h"

struct Auth
{
	std::string type;
	std::string username;
	std::string password;
};

Auth GetAuth(const shared_ptr< Session > session)
{
	Auth ret;
	std::string authorization = session->get_request()->get_header("Authorization");
	std::size_t breakPos = authorization.find(' ');
	ret.type = authorization.substr(0, breakPos);
	if (ret.type != "Basic")
		return ret;
	std::string b64 = authorization.substr(++breakPos);
	std::string b64Decoded;
	macaron::Base64::Decode(b64, b64Decoded);
	breakPos = b64Decoded.find(':');
	ret.username = b64Decoded.substr(0,breakPos);
	ret.password = b64Decoded.substr(++breakPos);
	//    fprintf(stdout, "%s (%s:%s => %s)\n", authorization.c_str(), ret.type.c_str(), b64.c_str(), b64Decoded.c_str());
	fprintf(stdout, "Username: \"%s\"\nPassword: \"%s\"\n", ret.username.c_str(), ret.password.c_str());
	return ret;
}
