#pragma once

#include "Precompiled.hpp"

//
// Component
//

namespace Game
{
    // Component base class.
    class Component
    {
    protected:
        Component()
        {
        }

    public:
        virtual ~Component()
        {
        }
    };
}
