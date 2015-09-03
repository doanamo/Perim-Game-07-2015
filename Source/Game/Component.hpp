#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Component
//

namespace Game
{
    // Component base class.
    class Component : private NonCopyable
    {
    protected:
        Component()
        {
        }

    public:
        virtual ~Component()
        {
        }

        virtual bool Finalize(EntityHandle self, const Context& context)
        {
            return true;
        }
    };
}
