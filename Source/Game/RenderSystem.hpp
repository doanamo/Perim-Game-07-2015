#pragma once

#include "Precompiled.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Shader.hpp"

// Forward declarations.
namespace System
{
    class Window;
}

namespace Graphics
{
    class BasicRenderer;
}

//
// Render System
//

namespace Game
{
    // Forward declarations.
    class ComponentSystem;

    // Render system class.
    class RenderSystem
    {
    public:
        RenderSystem();
        ~RenderSystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the render system instance.
        bool Initialize(Context& context);

        // Draws the scene.
        void Draw();

    private:
        // Context references.
        System::Window*          m_window;
        Graphics::BasicRenderer* m_basicRenderer;
        ComponentSystem*         m_componentSystem;

        // Graphics objects.
        Graphics::ScreenSpace  m_screenSpace;
        Graphics::VertexBuffer m_vertexBuffer;
        Graphics::VertexInput  m_vertexInput;
        Graphics::Shader       m_shader;

        // Initialization state.
        bool m_initialized;
    };
}
