#pragma once

#include "Precompiled.hpp"

//
// Script System
//

namespace Game
{
    // Forward declarations.
    class EntitySystem;
    class ComponentSystem;

    // Script system class.
    class ScriptSystem
    {
    public:
        ScriptSystem();
        ~ScriptSystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the script system instance.
        bool Initialize(Context& context);

        // Updates all script components.
        void Update(float timeDelta);

    private:
        // Context references.
        EntitySystem*    m_entitySystem;
        ComponentSystem* m_componentSystem;

        // Initialization state.
        bool m_initialized;
    };
}
