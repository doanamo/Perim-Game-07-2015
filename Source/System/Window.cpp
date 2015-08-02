#include "Precompiled.hpp"
#include "Window.hpp"
using namespace System;

namespace
{
    // Log message strings.
    #define LogInitializeError() "Failed to initialize a window! "

    // Instance counter for GLFW library.
    bool LibraryInitialized = false;
    int InstanceCount = 0;

    // Window callbacks.
    void ErrorCallback(int error, const char* description)
    {
        Log() << "GLFW Error: " << description;
    }

    void KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::KeyboardKey event;
        event.key = key;
        event.scancode = scancode;
        event.action = action;
        event.mods = mods;

        instance->keyboardKey(event);
    }

    void TextInputCallback(GLFWwindow* window, unsigned int character)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::TextInput event;
        event.character = character;

        instance->textInput(event);
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::MouseButton event;
        event.button = button;
        event.action = action;
        event.mods = mods;

        instance->mouseButton(event);
    }

    void MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::MouseScroll event;
        event.offset = offsety;

        instance->mouseScroll(event);
    }

    void CursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::CursorPosition event;
        event.x = x;
        event.y = y;

        instance->cursorPosition(event);
    }

    void CursorEnterCallback(GLFWwindow* window, int entered)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::CursorEnter event;
        event.entered = entered != 0;

        instance->cursorEnter(event);
    }
}

Window::Window() :
    m_window(nullptr),
    m_initialized(false)
{
    // Increase instance count.
    ++InstanceCount;
}

Window::~Window()
{
    // Decrease instance count.
    --InstanceCount;

    // Destroy the window.
    glfwDestroyWindow(m_window);

    // Shutdown GLFW library.
    if(InstanceCount == 0)
    {
        if(LibraryInitialized)
        {
            glfwTerminate();

            LibraryInitialized = false;
        }
    }
}

bool Window::Initialize()
{
    BOOST_ASSERT(!m_initialized);

    // Initialize GLFW library.
    if(!LibraryInitialized)
    {
        glfwSetErrorCallback(ErrorCallback);

        if(!glfwInit())
        {
            Log() << LogInitializeError() << "Couldn't initialize GLFW library!";
            return false;
        }

        LibraryInitialized = true;
    }

    // Create the window.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1024, 576, "Game", nullptr, nullptr);

    if(m_window == nullptr)
    {
        Log() << LogInitializeError() << "Couldn't create the window!";
        return false;
    }

    // Set window user data.
    glfwSetWindowUserPointer(m_window, this);

    // Add event callbacks.
    glfwSetKeyCallback(m_window, KeyboardKeyCallback);
    glfwSetCharCallback(m_window, TextInputCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetScrollCallback(m_window, MouseScrollCallback);
    glfwSetCursorPosCallback(m_window, CursorPositionCallback);
    glfwSetCursorEnterCallback(m_window, CursorEnterCallback);

    // Make window context current.
    glfwMakeContextCurrent(m_window);

    // Initialize GLEW library.
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if(error != GLEW_OK)
    {
        Log() << "GLEW Error: " << glewGetErrorString(error);
        Log() << LogInitializeError() << "Couldn't initialize GLEW library!";
        return false;
    }

    // Check created OpenGL context.
    int glMajor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MAJOR);
    int glMinor = glfwGetWindowAttrib(m_window, GLFW_CONTEXT_VERSION_MINOR);

    Log() << "Created OpenGL " << glMajor << "." << glMinor << " context.";

    // Success!
    return m_initialized = true;
}

void Window::MakeContextCurrent()
{
    BOOST_ASSERT(m_initialized);

    glfwMakeContextCurrent(m_window);
}

void Window::ProcessEvents()
{
    BOOST_ASSERT(m_initialized);

    glfwPollEvents();
}

void Window::Present(bool verticalSync)
{
    BOOST_ASSERT(m_initialized);

    glfwSwapInterval((int)verticalSync);
    glfwSwapBuffers(m_window);
}

bool Window::IsClosed() const
{
    BOOST_ASSERT(m_initialized);

    return glfwWindowShouldClose(m_window) != 0;
}

int Window::GetWidth() const
{
    BOOST_ASSERT(m_initialized);

    int width = 0;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    BOOST_ASSERT(m_initialized);

    int height = 0;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return height;
}

GLFWwindow* Window::GetPrivate()
{
    BOOST_ASSERT(m_initialized);
    return m_window;
}
