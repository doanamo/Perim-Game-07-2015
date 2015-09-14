#pragma once

#include "Precompiled.hpp"

//
// Sampler
//

namespace Graphics
{
    // Sampler class.
    class Sampler
    {
    public:
        Sampler();
        ~Sampler();

        // Restore instance to it's original state.
        void Cleanup();

        // Initializes the sampler object.
        bool Initialize();

        // Sets a sampler parameter.
        void SetParameter(GLenum parameter, GLint value);

        // Gets the sampler handle.
        GLuint GetHandle() const;

    private:
        // Sampler handle.
        GLuint m_handle;

        // Initialization state.
        bool m_initialized;
    };
}
