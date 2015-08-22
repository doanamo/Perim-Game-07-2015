#pragma once

#include "Precompiled.hpp"

//
// Basic Renderer
//

namespace Graphics
{
    // Clear flags.
    struct ClearFlags
    {
        enum Type
        {
            None    = 0,
            Color   = 1 << 0,
            Depth   = 1 << 1,
            Stencil = 1 << 2,
        };
    };

    // Basic renderer class.
    class BasicRenderer
    {
    public:
        BasicRenderer();
        ~BasicRenderer();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the basic renderer instance.
        bool Initialize(Context& context);

        // Clears the frame buffer.
        void Clear(uint32_t flags);

        // Sets the clear color.
        void SetClearColor(const glm::vec4& color);

        // Sets the clear depth.
        void SetClearDepth(float depth);

        // Sets the stencil depth.
        void SetClearStencil(int stencil);

    private:
        // Initialization state.
        bool m_initialized;
    };
}
