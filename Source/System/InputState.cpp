#include "Precompiled.hpp"
#include "InputState.hpp"
#include "Window.hpp"
using namespace System;

InputState::InputState() :
    m_initialized(false)
{
    this->Reset();
}

InputState::~InputState()
{
    if(m_initialized)
        this->Cleanup();
}

void InputState::Cleanup()
{
    // Reset input states.
    this->Reset();

    // Disconnect signals.
    m_keyboardKey.disconnect();
    m_windowFocus.disconnect();

    // Reset initialization state.
    m_initialized = false;
}

bool InputState::Initialize(Window& window)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

    // Connect event signals.
    m_keyboardKey = window.events.keyboardKey.connect([&](const Window::Events::KeyboardKey& event)
    {
        BOOST_ASSERT(0 <= event.key && event.key < KeyboardKeyCount);

        if(event.action == GLFW_PRESS)
        {
            m_keyboardState[event.key] = KeyboardKeyStates::Pressed;
        }
        else
        if(event.action == GLFW_RELEASE)
        {
            m_keyboardState[event.key] = KeyboardKeyStates::Released;
        }
    });

    m_windowFocus = window.events.focus.connect([&](const Window::Events::Focus& event)
    {
        if(!event.focused)
        {
            this->Reset();
        }
    });

    // Success!
    return m_initialized = true;
}

void InputState::Update()
{
    if(!m_initialized)
        return;

    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        auto& state = m_keyboardState[i];

        if(state == KeyboardKeyStates::Pressed)
        {
            state = KeyboardKeyStates::PressedRepeat;
        }
        else
        if(state == KeyboardKeyStates::Released)
        {
            state = KeyboardKeyStates::ReleasedRepeat;
        }
    }
}

void InputState::Reset()
{
    for(int i = 0; i < KeyboardKeyCount; ++i)
    {
        m_keyboardState[i] = KeyboardKeyStates::ReleasedRepeat;
    }
}

bool InputState::IsKeyDown(int key, bool repeat)
{
    if(!m_initialized)
        return false;

    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Pressed)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::PressedRepeat)
        return true;

    return false;
}

bool InputState::IsKeyUp(int key, bool repeat)
{
    if(!m_initialized)
        return false;

    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Released)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::ReleasedRepeat)
        return true;

    return false;
}
