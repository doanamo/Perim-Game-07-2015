#include "Precompiled.hpp"
#include "Render.hpp"
#include "Transform.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;
using namespace Components;

Render::Render() :
    m_transform(nullptr)
{
}

Render::~Render()
{
}

bool Render::Finalize(EntityHandle self, const Context& context)
{
    // Get required systems.
    ComponentSystem* componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();
    if(componentSystem == nullptr) return false;

    // Get required components.
    m_transform = componentSystem->Lookup<Transform>(self);
    if(m_transform == nullptr) return false;

    return true;
}

Transform* Render::GetTransform()
{
    return m_transform;
}
