/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Shell.cpp
    Game host shell - main context to operate between host and user-space.

===============================================================================
*/

#include "motor/Core/Shell.h"

namespace motor {

#ifdef MOTOR_HOST
static Shell shellLocal{};
Shell* shell = &shellLocal;
#else
Shell* shell = nullptr;
#endif

} // namespace motor
