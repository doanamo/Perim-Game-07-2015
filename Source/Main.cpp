#include "Precompiled.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Shader.hpp"

void ErrorCallback(int error, const char* description)
{
    Log() << "GLFW Error: " << description;
}

int main(int argc, char* argv[])
{
    // Initialize debug routines.
    Debug::Initialize();

    // Initialize build info.
    Build::Initialize();

    // Initialize logging system.
    Logger::Initialize();

    // Initialize GLFW library.
    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
    {
        Log() << "Failed to initialize GLFW library!";
        return -1;
    }

    BOOST_SCOPE_EXIT(&)
    {
        glfwTerminate();
    };

    // Create a window.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 576, "Game", nullptr, nullptr);

    if(window == nullptr)
    {
        Log() << "Failed to create a window!";
        return -1;
    }

    glfwMakeContextCurrent(window);

    BOOST_SCOPE_EXIT(&)
    {
        glfwDestroyWindow(window);
    };

    // Initialize GLEW library.
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        Log() << "GLEW Error: " << glewGetErrorString(error);
        Log() << "Failed to initialze GLEW library!";
        return -1;
    }

    // Check created OpenGL context.
    int glMajor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);

    Log() << "Created OpenGL " << glMajor << "." << glMinor << " context.";

    // Create a vertex buffer.
    const glm::vec3 vertices[] =
    {
        glm::vec3( 0.0f,  0.433f, 0.0f),
        glm::vec3(-0.5f, -0.433f, 0.0f),
        glm::vec3( 0.5f, -0.433f, 0.0f)
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

    // Main loop.
    while(!glfwWindowShouldClose(window))
    {
        // Setup viewport.
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        // Clear the back buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw scene.
        glBindVertexArray(vertexInput.GetHandle());
        BOOST_SCOPE_EXIT(&) { glBindVertexArray(0); };

        glUseProgram(shader.GetHandle());
        BOOST_SCOPE_EXIT(&) { glUseProgram(0); };

        glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap the back buffer.
        glfwSwapInterval(1);
        glfwSwapBuffers(window);

        // Process window events.
        glfwPollEvents();
    }

    return 0;
}
