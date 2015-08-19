#include "Precompiled.hpp"
#include "Player.hpp"
#include "System/InputState.hpp"
#include "Game/Components/Transform.hpp"
using namespace Game;
using namespace Scripts;

Player::Player(System::InputState* inputState, Components::Transform* transform) :
    m_inputState(inputState),
    m_transform(transform)
{
    BOOST_ASSERT(m_inputState != nullptr);
    BOOST_ASSERT(m_transform  != nullptr);
}

Player::~Player()
{
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
