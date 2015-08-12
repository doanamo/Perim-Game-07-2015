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

        // Initialized the script system.
        bool Initialize(Context& coreContext, Context& gameContext);

        // Updates all script components.
        void Update(float timeDelta);

    private:
        // Game systems.
        EntitySystem*    m_entitySystem;
        ComponentSystem* m_componentSystem;

        // Initialization state.
        bool m_initialized;
    };
}
