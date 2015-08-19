#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 viewTransform;

    layout(location = 0) in vec3 vertexPosition;

    void main()
    {
        gl_Position = viewTransform * vec4(vertexPosition, 1.0f);
    }
#endif

#if defined(FRAGMENT_SHADER)
    uniform vec4 fragmentDiffuseColor;

    out vec4 finalColor;

    void main()
    {
        finalColor = fragmentDiffuseColor;
    }
#endif
