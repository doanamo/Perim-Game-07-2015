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
        bool Initialize(Context& coreContext, Context& gameContext);

        // Draws the scene.
        void Draw();

    private:
        // Initialization state.
        bool m_initialized;
    };
}
