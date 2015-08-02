#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Transform.hpp"
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

bool RenderSystem::Initialize(Context& coreContext, Context& gameContext)
{
    BOOST_ASSERT(!m_initialized);

    // Add system to the context.
    BOOST_ASSERT(gameContext.Set(this));

    // Get required systems.
    m_window = coreContext.Get<System::Window>();
    BOOST_ASSERT_MSG(m_window != nullptr, "Context is missing Window instance.");

    m_componentSystem = gameContext.Get<ComponentSystem>();
    BOOST_ASSERT_MSG(m_componentSystem != nullptr, "Context is missing ComponentSystem instance.");

    // Declare required components.
    m_componentSystem->Declare<Game::Components::Transform>();

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

    // Clear the back buffer.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    Components::Transform& transform = m_componentSystem->Begin<Components::Transform>()->second;

    // Calculate camera view.
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -glm::vec3(m_screenSpace.GetOffset(), 0.0f));

    // Draw scene.
    glBindVertexArray(m_vertexInput.GetHandle());
    BOOST_SCOPE_EXIT(&) { glBindVertexArray(0); };

    glUseProgram(m_shader.GetHandle());
    BOOST_SCOPE_EXIT(&) { glUseProgram(0); };

    glm::mat4 vertexTransform = transform.CalculateMatrix(m_screenSpace.GetTransform() * view);
    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(vertexTransform));

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
