#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;

    layout(location = 0) in vec3 vertexPosition;

    void main()
    {
        gl_Position = vertexTransform * vec4(vertexPosition, 1.0f);
    }
#endif

#if defined(FRAGMENT_SHADER)
    out vec4 finalColor;

    void main()
    {
        finalColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
#endif
