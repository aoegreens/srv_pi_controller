#pragma once

#include "web/common/Parameter.h"
#include "common/GlobalPinController.h"

namespace aoe {
namespace controller {

inline void SetGPIO(const std::shared_ptr< restbed::Session > session);

} //controller namespace
} //aoe namespace
