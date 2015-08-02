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

    private:
        // Window implementation.
        GLFWwindow* m_window;

        // Initialization state.
        bool m_initialized;
    };
}
