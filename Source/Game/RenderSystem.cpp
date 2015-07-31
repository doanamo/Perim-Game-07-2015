#include "Precompiled.hpp"
#include "RenderSystem.hpp"
using namespace Game;

RenderSystem::RenderSystem() :
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::Initialize(Context& coreContext, Context& gameContext)
{
    BOOST_ASSERT(!m_initialized);

    // Success!
    return m_initialized = true;
}

void RenderSystem::Draw()
{
}
