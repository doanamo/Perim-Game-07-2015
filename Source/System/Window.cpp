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

    void MoveCallback(GLFWwindow* window, int x, int y)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::Move event;
        event.x = x;
        event.y = y;

        instance->events.move(event);
    }

    void ResizeCallback(GLFWwindow* window, int width, int height)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::Resize event;
        event.width = width;
        event.height = height;

        instance->events.resize(event);
    }

    void FocusCallback(GLFWwindow* window, int focused)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send and event.
        Window::Events::Focus event;
        event.focused = focused > 0;
        
        instance->events.focus(event);
    }

    void CloseCallback(GLFWwindow* window)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send and event.
        Window::Events::Close event;
        
        instance->events.close(event);
    }

    void KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::KeyboardKey event;
        event.key = key;
        event.scancode = scancode;
        event.action = action;
        event.mods = mods;

        instance->events.keyboardKey(event);
    }

    void TextInputCallback(GLFWwindow* window, unsigned int character)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::TextInput event;
        event.character = character;

        instance->events.textInput(event);
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::MouseButton event;
        event.button = button;
        event.action = action;
        event.mods = mods;

        instance->events.mouseButton(event);
    }

    void MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::MouseScroll event;
        event.offset = offsety;

        instance->events.mouseScroll(event);
    }

    void CursorPositionCallback(GLFWwindow* window, double x, double y)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::CursorPosition event;
        event.x = x;
        event.y = y;

        instance->events.cursorPosition(event);
    }

    void CursorEnterCallback(GLFWwindow* window, int entered)
    {
        // Get window instance.
        Window* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        BOOST_ASSERT(instance != nullptr);

        // Send an event.
        Window::Events::CursorEnter event;
        event.entered = entered != 0;

        instance->events.cursorEnter(event);
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
    // Cleanup instance.
    if(m_initialized)
        this->Cleanup();

    // Decrease instance count.
    --InstanceCount;

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

void Window::Cleanup()
{
    // Destroy the window.
    if(m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    // Reset event signals.
    this->events.move.disconnect_all_slots();
    this->events.resize.disconnect_all_slots();
    this->events.focus.disconnect_all_slots();
    this->events.close.disconnect_all_slots();
    this->events.keyboardKey.disconnect_all_slots();
    this->events.textInput.disconnect_all_slots();
    this->events.mouseButton.disconnect_all_slots();
    this->events.mouseScroll.disconnect_all_slots();
    this->events.cursorPosition.disconnect_all_slots();
    this->events.cursorEnter.disconnect_all_slots();

    // Reset initialization state.
    m_initialized = false;
}

bool Window::Initialize(int width, int height)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

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

    m_window = glfwCreateWindow(width, height, "Game", nullptr, nullptr);

    if(m_window == nullptr)
    {
        Log() << LogInitializeError() << "Couldn't create the window!";
        return false;
    }

    // Set window user data.
    glfwSetWindowUserPointer(m_window, this);

    // Add event callbacks.
    glfwSetWindowPosCallback(m_window, MoveCallback);
    glfwSetFramebufferSizeCallback(m_window, ResizeCallback);
    glfwSetWindowFocusCallback(m_window, FocusCallback);
    glfwSetWindowCloseCallback(m_window, CloseCallback);
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
    if(!m_initialized)
        return;

    glfwMakeContextCurrent(m_window);
}

void Window::ProcessEvents()
{
    if(!m_initialized)
        return;

    glfwPollEvents();
}

void Window::Present(bool verticalSync)
{
    if(!m_initialized)
        return;

    glfwSwapInterval((int)verticalSync);
    glfwSwapBuffers(m_window);
}

void Window::Close()
{
    if(!m_initialized)
        return;

    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

bool Window::IsOpen() const
{
    if(!m_initialized)
        return false;

    return glfwWindowShouldClose(m_window) == 0;
}

bool Window::IsFocused() const
{
    if(!m_initialized)
        return false;

    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) > 0;
}

int Window::GetWidth() const
{
    if(!m_initialized)
        return 0;

    // Get frame buffer width.
    int width = 0;
    glfwGetFramebufferSize(m_window, &width, nullptr);
    return width;
}

int Window::GetHeight() const
{
    if(!m_initialized)
        return 0;

    // Get frame buffer height.
    int height = 0;
    glfwGetFramebufferSize(m_window, nullptr, &height);
    return height;
}

GLFWwindow* Window::GetPrivate()
{
    return m_window;
}
