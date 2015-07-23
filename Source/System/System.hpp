#pragma once

#include "Precompiled.hpp"

//
// System
//

namespace System
{
    // Initializes the system.
    bool Initialize();

    // Cleanups the system.
    void Cleanup();

    // Gets the window.
    GLFWwindow* GetWindow();
}
