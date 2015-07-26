#include "Precompiled.hpp"
#include "RenderSystem.hpp"
using namespace Game;

RenderSystem::RenderSystem() :
m_initialized()
{
}

RenderSystem::~RenderSystem()
{
    Cleanup();
}

bool RenderSystem::Initialize()
{
    Cleanup();

    // Success!
    return m_initialized = true;
}

void RenderSystem::Cleanup()
{
    m_initialized = false;
}

void RenderSystem::Draw()
{
}