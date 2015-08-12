#pragma once

#include "Precompiled.hpp"

//
// Window
//

namespace System
{
    // Window class.
    class Window
    {
    public:
        Window();
        ~Window();

        // Creates the window.
        bool Initialize();

        // Makes window's context current.
        void MakeContextCurrent();

        // Processes window events.
        void ProcessEvents();

        // Presents backbuffer content to the window.
        void Present(bool verticalSync);

        // Checks if window has been closed.
        bool IsClosed() const;

        // Gets window's width.
        int GetWidth() const;

        // Gets window'd height.
        int GetHeight() const;

        // Gets window's private data.
        GLFWwindow* GetPrivate();

    public:
        // Event signals.
        struct Events
        {
            // Keyboard key signal.
            struct KeyboardKey
            {
                int key;
                int scancode;
                int action;
                int mods;
            };

            boost::signals2::signal<void(const KeyboardKey&)> keyboardKey;

            // Text input signal.
            struct TextInput
            {
                unsigned int character;
            };

            boost::signals2::signal<void(const TextInput&)> textInput;

            // Mouse button signal.
            struct MouseButton
            {
                int button;
                int action;
                int mods;
            };

            boost::signals2::signal<void(const MouseButton&)> mouseButton;

            // Mouse scroll signal.
            struct MouseScroll
            {
                double offset;
            };

            boost::signals2::signal<void(const MouseScroll&)> mouseScroll;

            // Cursor position signal.
            struct CursorPosition
            {
                double x;
                double y;
            };

            boost::signals2::signal<void(const CursorPosition&)> cursorPosition;

            // Cursor enter signal.
            struct CursorEnter
            {
                bool entered;
            };

            boost::signals2::signal<void(const CursorEnter&)> cursorEnter;
        } events;

    private:
        // Window implementation.
        GLFWwindow* m_window;

        // Initialization state.
        bool m_initialized;
    };
}
