#include "Precompiled.hpp"
#include "BasicRenderer.hpp"
using namespace Graphics;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the basic renderer! "
}

BasicRenderer::BasicRenderer()
{
}

BasicRenderer::~BasicRenderer()
{
    if(m_initialized)
        this->Cleanup();
}

void BasicRenderer::Cleanup()
{
    // Reset initialization state.
    m_initialized = false;
}

bool BasicRenderer::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

    // Add instance to the context.
    if(context[ContextTypes::Main].Has<BasicRenderer>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Main].Set(this);

    // Success!
    return m_initialized = true;
}

void BasicRenderer::Clear(uint32_t flags)
{
    if(!m_initialized)
        return;

    // Clear the frame buffer.
    GLbitfield mask = GL_NONE;

    if(flags & ClearFlags::Color)   mask |= GL_COLOR_BUFFER_BIT;
    if(flags & ClearFlags::Depth)   mask |= GL_DEPTH_BUFFER_BIT;
    if(flags & ClearFlags::Stencil) mask |= GL_STENCIL_BUFFER_BIT;

    glClear(mask);
}

void BasicRenderer::SetClearColor(const glm::vec4& color)
{
    if(!m_initialized)
        return;

    glClearColor(color.r, color.g, color.b, color.a);
}

void BasicRenderer::SetClearDepth(float depth)
{
    if(!m_initialized)
        return;

    glClearDepth(depth);
}

void BasicRenderer::SetClearStencil(int stencil)
{
    if(!m_initialized)
        return;

    glClearStencil(stencil);
}
