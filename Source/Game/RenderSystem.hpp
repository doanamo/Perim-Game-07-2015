#pragma once

#include "Precompiled.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/BasicRenderer.hpp"

// Forward declarations.
namespace System
{
    class Window;
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
        // Type delcarations.
        typedef std::vector<Graphics::BasicRenderer::Sprite::Info> SpriteInfoList;
        typedef std::vector<Graphics::BasicRenderer::Sprite::Data> SpriteDataList;

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

        // Sprite list.
        SpriteInfoList m_spriteInfo;
        SpriteDataList m_spriteData;

        // Initialization state.
        bool m_initialized;
    };
}
