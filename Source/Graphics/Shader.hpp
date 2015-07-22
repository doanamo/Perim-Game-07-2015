#pragma once

#include "Precompiled.hpp"

//
// Shader
//
//  Loads and links different OpenGL shaders into a program object.
//
//  Shader code example:
//      #version 330
//      
//      #if defined(VERTEX_SHADER)
//          uniform mat4 vertexTransform;
//      
//          layout(location = 0) in vec3 vertexPosition;
//      
//          void main()
//          {
//              gl_Position = vertexTransform * vec4(vertexPosition, 1.0f);
//          }
//      #endif
//      
//      #if defined(FRAGMENT_SHADER)
//          out vec4 finalColor;
//      
//          void main()
//          {
//              finalColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//          }
//      #endif
//
//  Loading and using a shader.
//      Graphics::Shader shader;
//      shader.Load("Data/Shader.glsl");
//      
//      glUseProgram(shader.GetHandle());
//      glUniformMatrix4fv(shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
//

namespace Graphics
{
    // Shader class.
    class Shader
    {
    public:
        Shader();
        ~Shader();

        // Loads the shader from a file.
        bool Load(std::string filename);

        // Restores object instance to it's previous state.
        void Cleanup();

        // Gets the shader's program handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Gets a shader attribute index.
        GLint GetAttribute(std::string name) const;

        // Gets a shader uniform index.
        GLint GetUniform(std::string name) const;

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Linked program handle.
        GLuint m_handle;
    };
}
