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
}

bool InputState::Initialize(Window& window)
{
    BOOST_ASSERT(!m_initialized);

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
    BOOST_ASSERT(m_initialized);

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
    if(key < 0 || key >= KeyboardKeyCount)
        return false;

    if(m_keyboardState[key] == KeyboardKeyStates::Released)
        return true;

    if(repeat && m_keyboardState[key] == KeyboardKeyStates::ReleasedRepeat)
        return true;

    return false;
}
