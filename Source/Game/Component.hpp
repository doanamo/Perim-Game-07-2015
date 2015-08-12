#pragma once

#include "Precompiled.hpp"

//
// Component
//

namespace Game
{
    // Component base class.
    class Component : private boost::noncopyable
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
