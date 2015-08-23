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
    m_vertexBuffer.Cleanup();
    m_vertexInput.Cleanup();
    m_texture.Cleanup();
    m_shader.Cleanup();

    // Reset initialization state.
    m_initialized = false;
}

bool RenderSystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

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

    // Declare required components.
    m_componentSystem->Declare<Game::Components::Transform>();
    m_componentSystem->Declare<Game::Components::Render>();

    // Set screen space target size.
    m_screenSpace.SetTargetSize(10.0f, 10.0f);

    // Initialize the vertex buffer.
    const glm::vec3 vertices[] =
    {
        { 0.0f, 1.0f, 0.0f, },
        { 1.0f, 0.0f, 0.0f, },
        { 0.0f, 0.0f, 0.0f, },

        { 0.0f, 1.0f, 0.0f, },
        { 1.0f, 1.0f, 0.0f, },
        { 1.0f, 0.0f, 0.0f, },
    };

    if(!m_vertexBuffer.Initialize(sizeof(glm::vec3), boost::size(vertices), &vertices[0]))
    {
        Log() << LogInitializeError() << "Couldn't initialize the vertex buffer.";
        return false;
    }

    // Initialize the vertex input.
    const Graphics::VertexAttribute vertexAttributes[] =
    {
        { &m_vertexBuffer, Graphics::VertexAttributeTypes::Float3 },
    };

    if(!m_vertexInput.Initialize(boost::size(vertexAttributes), &vertexAttributes[0]))
    {
        Log() << LogInitializeError() << "Couldn't initialize the vertex input.";
        return false;
    }

    // Load the texture.
    if(!m_texture.Load(Build::GetWorkingDir() + "Data/Textures/Check.png"))
    {
        Log() << LogInitializeError() << "Couldn't load the texture.";
        return false;
    }

    // Load the shader.
    if(!m_shader.Load(Build::GetWorkingDir() + "Data/Shaders/Basic.glsl"))
    {
        Log() << LogInitializeError() << "Couldn't load the shader.";
        return false;
    }

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

    // Clear the back buffer.
    m_basicRenderer->SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_basicRenderer->SetClearDepth(1.0f);

    m_basicRenderer->Clear(Graphics::ClearFlags::Color | Graphics::ClearFlags::Depth);

    // Set render states.
    glBindVertexArray(m_vertexInput.GetHandle());
    BOOST_SCOPE_EXIT(&) { glBindVertexArray(0); };

    glUseProgram(m_shader.GetHandle());
    BOOST_SCOPE_EXIT(&) { glUseProgram(0); };

    // Iterate over all render components.
    auto componentsBegin = m_componentSystem->Begin<Components::Render>();
    auto componentsEnd = m_componentSystem->End<Components::Render>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Get the components.
        Components::Render* render = &it->second;
        BOOST_ASSERT(render != nullptr);

        Components::Transform* transform = render->GetTransform();
        BOOST_ASSERT(transform != nullptr);

        // Draw entity.
        glm::mat4 vertexTransform = transform->CalculateMatrix(m_screenSpace.GetTransform() * view);
        glUniformMatrix4fv(m_shader.GetUniform("viewTransform"), 1, GL_FALSE, glm::value_ptr(vertexTransform));
        glUniform4fv(m_shader.GetUniform("fragmentDiffuseColor"), 1, glm::value_ptr(render->GetDiffuseColor()));
        glUniform4fv(m_shader.GetUniform("fragmentEmissiveColor"), 1, glm::value_ptr(render->GetEmissiveColor()));
        glUniform1f(m_shader.GetUniform("fragmentEmissivePower"), render->GetEmissivePower());

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Draw sprites.
    Graphics::BasicRenderer::Sprite sprite;
    sprite.info.texture = &m_texture;
    sprite.info.transparent = false;
    sprite.data.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 0.0f));
    sprite.data.transform = glm::scale(sprite.data.transform, glm::vec3(1.0 / 128.0f, 1.0 / 128.0f, 1.0f));
    sprite.data.rectangle = glm::vec4(0.0f, 0.0f, m_texture.GetWidth(), m_texture.GetHeight());
    sprite.data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    m_basicRenderer->DrawSprites(&sprite, 1, m_screenSpace.GetTransform() * view);
}
