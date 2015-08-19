#include "Precompiled.hpp"
#include "Player.hpp"
#include "System/InputState.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Components/Transform.hpp"
using namespace Game;
using namespace Scripts;

Player::Player() :
    m_inputState(nullptr),
    m_transform(nullptr)
{
}

Player::~Player()
{
}

bool Player::OnFinalize(EntityHandle self, const Context& context)
{
    // Get required systems.
    m_inputState = context[ContextTypes::Main].Get<System::InputState>();
    if(m_inputState == nullptr) return false;

    ComponentSystem* componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();
    if(componentSystem == nullptr) return false;

    // Get required components.
    m_transform = componentSystem->Lookup<Components::Transform>(self);
    if(m_transform == nullptr) return false;

    return true;
}

void Player::OnUpdate(EntityHandle self, float timeDelta)
{
    // Move entity.
    glm::vec2 direction;

    if(m_inputState->IsKeyDown(GLFW_KEY_D))
        direction.x += 1.0f;

    if(m_inputState->IsKeyDown(GLFW_KEY_A))
        direction.x -= 1.0f;

    if(m_inputState->IsKeyDown(GLFW_KEY_W))
        direction.y += 1.0f;

    if(m_inputState->IsKeyDown(GLFW_KEY_S))
        direction.y -= 1.0f;

    if(direction != glm::vec2(0.0f, 0.0f))
    {
        glm::vec2 position = m_transform->GetPosition();
        position += glm::normalize(direction) * 6.0f * timeDelta;
        m_transform->SetPosition(position);
    }
}
