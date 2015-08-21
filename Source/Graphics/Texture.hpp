#pragma once

#include "Precompiled.hpp"

//
// Texture
//

namespace Graphics
{
    // Texture class.
    class Texture
    {
    public:
        Texture();
        ~Texture();

        // Initializes the texture.
        bool Initialize(int width, int height, GLenum format, const void* data);

        // Updates the texture data.
        void Update(const void* data);

        // Gets the texture handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Gets the texture width.
        int GetWidth() const
        {
            return m_width;
        }

        // Gets the texture height.
        int GetHeight() const
        {
            return m_height;
        }

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Texture handle.
        GLuint m_handle;

        // Texture parameters.
        int m_width;
        int m_height;
        GLenum m_format;

        // Initialization state.
        bool m_initialized;
    };
}
