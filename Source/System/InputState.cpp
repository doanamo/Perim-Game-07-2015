#include "Precompiled.hpp"
#include "InputState.hpp"
#include "Window.hpp"
using namespace System;

InputState::InputState() :
    m_initialized(false)
{
    this->Reset();

    // Bind event receivers.
    m_keyboardKey.Bind<InputState, &InputState::OnKeyboardKey>(this);
    m_windowFocus.Bind<InputState, &InputState::OnWindowFocus>(this);
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

    // Unsubscribe event receivers.
    m_keyboardKey.Unsubscribe();
    m_windowFocus.Unsubscribe();

    // Reset initialization state.
    m_initialized = false;
}

bool InputState::Initialize(Window& window)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Subscribe event receivers.
    window.events.keyboardKey.Subscribe(m_keyboardKey);
    window.events.focus.Subscribe(m_windowFocus);

    // Success!
    return m_initialized = true;
}

void InputState::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    assert(0 <= event.key && event.key < KeyboardKeyCount);

    if(event.action == GLFW_PRESS)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Pressed;
    }
    else
    if(event.action == GLFW_RELEASE)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Released;
    }
}

void InputState::OnWindowFocus(const Window::Events::Focus& event)
{
    if(!event.focused)
    {
        this->Reset();
    }
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
