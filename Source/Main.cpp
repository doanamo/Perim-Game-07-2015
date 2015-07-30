#include "Precompiled.hpp"
#include "System/Window.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Components/Transform.hpp"
#include "Game/IdentitySystem.hpp"
#include "Game/RenderSystem.hpp"

int main(int argc, char* argv[])
{
    // Initialize debug routines.
    Debug::Initialize();

    // Initialize the build info.
    Build::Initialize();

    // Initialize the logger.
    Logger::Initialize();

    // Initialize the window.
    System::Window window;
    if(!window.Initialize())
    {
        return -1;
    }

    // Initialize the entity system.
    Game::EntitySystem entitySystem;
    if(!entitySystem.Initialize())
    {
        return -1;
    }

    // Create a screen space.
    Graphics::ScreenSpace screenSpace;
    screenSpace.SetTargetSize(10.0f, 10.0f);

    // Create a vertex buffer.
    const glm::vec3 vertices[] =
    {
        { 0.0f, 1.0f, 0.0f, },
        { 1.0f, 0.0f, 0.0f, },
        { 0.0f, 0.0f, 0.0f, },

        { 0.0f, 1.0f, 0.0f, },
        { 1.0f, 1.0f, 0.0f, },
        { 1.0f, 0.0f, 0.0f, },
    };

    Graphics::VertexBuffer vertexBuffer;
    if(!vertexBuffer.Initialize(sizeof(glm::vec3), boost::size(vertices), &vertices[0]))
    {
        return -1;
    }

    // Create a vertex input.
    const Graphics::VertexAttribute vertexAttributes[] =
    {
        { &vertexBuffer, Graphics::VertexAttributeTypes::Float3 },
    };

    Graphics::VertexInput vertexInput;
    if(!vertexInput.Initialize(boost::size(vertexAttributes), &vertexAttributes[0]))
    {
        return -1;
    }

    // Load a shader.
    Graphics::Shader shader;
    if(!shader.Load(Build::GetWorkingDir() + "Data/Shaders/Basic.glsl"))
    {
        return -1;
    }

    // Initialize the component system.
    Game::ComponentSystem componentSystem;
    if(!componentSystem.Initialize())
    {
        return -1;
    }

    componentSystem.ConnectEntityDestroyed(entitySystem.entityDestroyed);

    // Initialize the identity system.
    Game::IdentitySystem identitySystem;
    if(!identitySystem.Initialize())
    {
        return -1;
    }

    identitySystem.ConnectEntityDestroyed(entitySystem.entityDestroyed);

    // Initialize the render system.
    Game::RenderSystem renderSystem;
    if(!renderSystem.Initialize())
    {
        return -1;
    }

    // Declare components.
    componentSystem.Declare<Game::Components::Transform>();

    // Create an entity.
    Game::EntityHandle entity = entitySystem.CreateEntity();
    identitySystem.SetEntityName(entity, "Player");

    auto transform = componentSystem.Create<Game::Components::Transform>(entity);
    transform->SetPosition(glm::vec2(2.0f, 2.0f));

    // Main loop.
    window.MakeContextCurrent();

    while(!window.IsClosed())
    {
        // Process window events.
        window.ProcessEvents();

        // Process entity commands.
        entitySystem.ProcessCommands();

        // Setup viewport.
        glViewport(0, 0, window.GetWidth(), window.GetHeight());

        // Setup screen space.
        screenSpace.SetSourceSize(window.GetWidth(), window.GetHeight());

        // Clear the back buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw scene.
        glBindVertexArray(vertexInput.GetHandle());
        BOOST_SCOPE_EXIT(&) { glBindVertexArray(0); };

        glUseProgram(shader.GetHandle());
        BOOST_SCOPE_EXIT(&) { glUseProgram(0); };

        glm::mat4 vertexTransform = transform->CalculateMatrix(screenSpace.GetTransform());
        glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(vertexTransform));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Present backbuffer to the window.
        window.Present(true);
    }

    return 0;
}
