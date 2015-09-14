#pragma once

#include "Precompiled.hpp"

//
// Animation System
//

namespace Game
{
    // Forward declarations.
    class ComponentSystem;

    // Animation system class.
    class AnimationSystem
    {
    public:
        AnimationSystem();
        ~AnimationSystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the animation system.
        bool Initialize(Context& context);

        // Updates all animation components.
        void Update(float timeDelta);

    private:
        // Context references.
        ComponentSystem* m_componentSystem;

        // Initialization state.
        bool m_initialized;
    };
}
