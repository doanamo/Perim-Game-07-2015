#pragma once

#include "Precompiled.hpp"

//
// Render System
//

namespace Game
{
    // Render system class.
    class RenderSystem
    {
    public:
        RenderSystem();
        ~RenderSystem();

        // Initializes the render system.
        bool Initialize();

        // Restores class instance to it's original state.
        void Cleanup();

        // Draws the scene.
        void Draw();

    private:
        // System state.
        bool m_initialized;
    };
}
