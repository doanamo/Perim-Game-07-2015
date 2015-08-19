#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Transform.hpp"
#include "Components/Render.hpp"
#include "System/Window.hpp"
using namespace Game;

namespace
{
    // Log messages.
    #define LogInitializeError() "Failed to initialize the render system! "
}

RenderSystem::RenderSystem() :
    m_window(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::Initialize(Context& context)
{
    BOOST_ASSERT(!m_initialized);

    // Add system to the context.
    BOOST_ASSERT(context[ContextTypes::Game].Set(this));

    // Get required systems.
    m_window = context[ContextTypes::Main].Get<System::Window>();
    BOOST_ASSERT_MSG(m_window != nullptr, "Context is missing Window instance.");

    m_componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();
    BOOST_ASSERT_MSG(m_componentSystem != nullptr, "Context is missing ComponentSystem instance.");

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
    BOOST_ASSERT(m_initialized);

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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
