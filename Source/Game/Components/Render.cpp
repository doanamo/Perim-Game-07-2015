#include "Precompiled.hpp"
#include "Render.hpp"
#include "Transform.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;
using namespace Components;

Render::Render() :
    m_transform(nullptr),
    m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
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

void Render::SetDiffuseColor(const glm::vec4& color)
{
    m_diffuseColor = color;
}

Transform* Render::GetTransform()
{
    return m_transform;
}

const glm::vec4& Render::GetDiffuseColor() const
{
    return m_diffuseColor;
}
