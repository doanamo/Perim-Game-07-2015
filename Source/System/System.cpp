#include "Precompiled.hpp"
#include "System.hpp"

namespace
{
    // Application window.
    GLFWwindow* window = nullptr;
}

void ErrorCallback(int error, const char* description)
{
    Log() << "GLFW Error: " << description;
}

bool System::Initialize()
{
    Cleanup();

    // Initialize GLFW library.
    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
    {
        Log() << "Failed to initialize GLFW library!";
        return false;
    }

    // Create a window.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 576, "Game", nullptr, nullptr);

    if(window == nullptr)
    {
        Log() << "Failed to create a window!";
        return false;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW library.
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        Log() << "GLEW Error: " << glewGetErrorString(error);
        Log() << "Failed to initialze GLEW library!";
        return false;
    }

    // Check created OpenGL context.
    int glMajor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);

    Log() << "Created OpenGL " << glMajor << "." << glMinor << " context.";

    // Success!
    return true;
}

void System::Cleanup()
{
    // Destroy the window.
    glfwDestroyWindow(window);
    window = nullptr;

    // Shutdown GLFW library.
    glfwTerminate();
}

GLFWwindow* System::GetWindow()
{
    return window;
}
