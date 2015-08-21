#pragma once

#include "Precompiled.hpp"

//
// Input State
//

namespace System
{
    // Forward declarations.
    class Window;

    // Input state class.
    class InputState
    {
    public:
        // Constant variables.
        static const int KeyboardKeyCount = GLFW_KEY_LAST + 1;

        // Keyboard key state.
        struct KeyboardKeyStates
        {
            enum Type
            {
                Pressed,
                PressedRepeat,
                Released,
                ReleasedRepeat,
            };
        };

    public:
        InputState();
        ~InputState();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the input state instance.
        bool Initialize(Window& window);

        // Updates the input state.
        void Update();
        
        // Resets the input state.
        void Reset();

        // Checks if a key is down.
        bool IsKeyDown(int key, bool repeat = true);

        // Checks if a key is up.
        bool IsKeyUp(int key, bool repeat = true);

    private:
        // Table of keyboard key states.
        KeyboardKeyStates::Type m_keyboardState[KeyboardKeyCount];

        // Signal connections.
        boost::signals2::scoped_connection m_keyboardKey;
        boost::signals2::scoped_connection m_windowFocus;

        // Initialization state.
        bool m_initialized;
    };
}
