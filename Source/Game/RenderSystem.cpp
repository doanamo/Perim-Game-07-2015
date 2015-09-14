#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "System/Window.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "ComponentSystem.hpp"
#include "Components/Transform.hpp"
#include "Components/Render.hpp"
using namespace Game;

namespace
{
    // Log messages.
    #define LogInitializeError() "Failed to initialize the render system! "
}

RenderSystem::RenderSystem() :
    m_window(nullptr),
    m_basicRenderer(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
    if(m_initialized)
        this->Cleanup();
}

void RenderSystem::Cleanup()
{
    // Reset context references.
    m_window = nullptr;
    m_basicRenderer = nullptr;
    m_componentSystem = nullptr;

    // Reset graphics objects.
    m_screenSpace.Cleanup();

    // Cleanup sprite list.
    Utility::ClearContainer(m_spriteInfo);
    Utility::ClearContainer(m_spriteData);

    // Reset initialization state.
    m_initialized = false;
}

bool RenderSystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Add instance to the context.
    if(context[ContextTypes::Game].Has<RenderSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Game].Set(this);

    // Get the window.
    m_window = context[ContextTypes::Main].Get<System::Window>();

    if(m_window == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing Window instance.";
        return false;
    }

    // Get the basic renderer.
    m_basicRenderer = context[ContextTypes::Main].Get<Graphics::BasicRenderer>();

    if(m_basicRenderer == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing BasicRenderer instance.";
        return false;
    }

    // Get the component system.
    m_componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();

    if(m_componentSystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing ComponentSystem instance.";
        return false;
    }

    // Set screen space target size.
    m_screenSpace.SetTargetSize(10.0f, 10.0f);

    // Allocate initial sprite list memory.
    const int SpriteListSize = 128;
    m_spriteInfo.reserve(SpriteListSize);
    m_spriteData.reserve(SpriteListSize);

    // Success!
    return m_initialized = true;
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    // Get window size.
    int windowWidth = m_window->GetWidth();
    int windowHeight = m_window->GetHeight();

    // Setup viewport.
    glViewport(0, 0, windowWidth, windowHeight);

    // Setup screen space.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);

    // Calculate camera view.
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -glm::vec3(m_screenSpace.GetOffset(), 0.0f));

    // Global rendering scale.
    glm::vec3 renderScale(1.0 / 128.0f, 1.0 / 128.0f, 1.0f);

    // Clear the back buffer.
    m_basicRenderer->SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_basicRenderer->SetClearDepth(1.0f);

    m_basicRenderer->Clear(Graphics::ClearFlags::Color | Graphics::ClearFlags::Depth);

    // Iterate over all render components.
    auto componentsBegin = m_componentSystem->Begin<Components::Render>();
    auto componentsEnd = m_componentSystem->End<Components::Render>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Get the components.
        Components::Render* render = &it->second;
        assert(render != nullptr);

        Components::Transform* transform = render->GetTransform();
        assert(transform != nullptr);

        // Add sprite to the list.
        Graphics::BasicRenderer::Sprite::Info info;
        info.texture = render->GetTexture().get();
        info.transparent = render->IsTransparent();
        info.filter = false;

        Graphics::BasicRenderer::Sprite::Data data;
        data.transform = glm::scale(transform->CalculateMatrix(), renderScale);
        data.rectangle = render->GetRectangle();
        data.color = render->CalculateColor();

        m_spriteInfo.push_back(info);
        m_spriteData.push_back(data);
    }

    // Render sprites.
    m_basicRenderer->DrawSprites(m_spriteInfo, m_spriteData, m_screenSpace.GetTransform() * view);

    // Clear the sprite list.
    m_spriteInfo.clear();
    m_spriteData.clear();
}
