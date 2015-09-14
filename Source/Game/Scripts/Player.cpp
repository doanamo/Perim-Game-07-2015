#include "Precompiled.hpp"
#include "Player.hpp"
#include "System/InputState.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Components/Transform.hpp"
#include "Game/Components/Animation.hpp"
using namespace Game;
using namespace Scripts;

Player::Player() :
    m_inputState(nullptr),
    m_transform(nullptr),
    m_animation(nullptr)
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

    m_animation = componentSystem->Lookup<Components::Animation>(self);
    if(m_animation == nullptr) return false;

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

    if(direction != glm::vec2(0.0f))
    {
        glm::vec2 position = m_transform->GetPosition();
        float rotation = m_transform->GetRotation();

        // Calculate new position.
        position += glm::normalize(direction) * 2.0f * timeDelta;
        m_transform->SetPosition(position);

        // Calculate new rotation.
        glm::vec2 heading;
        heading.x = glm::sin(glm::radians(rotation));
        heading.y = glm::cos(glm::radians(rotation));

        rotation += glm::degrees(glm::orientedAngle(glm::normalize(direction), heading));
        m_transform->SetRotation(rotation);

        // Play moving animation.
        if(330.0f < rotation || rotation <= 30.0f)
        {
            m_animation->Play("moving_up", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
        else
        if(30.0f < rotation && rotation <= 150.0f)
        {
            m_animation->Play("moving_right", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
        else
        if(150.0f < rotation && rotation <= 210.0f)
        {
            m_animation->Play("moving_down", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
        else
        if(210.0f < rotation && rotation <= 330.0f)
        {
            m_animation->Play("moving_left", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
    }
    else
    {
        // Play standing animation.
        float rotation = m_transform->GetRotation();

        if(330.0f < rotation || rotation <= 30.0f)
        {
            m_animation->Play("standing_up", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
        else
        if(30.0f < rotation && rotation <= 150.0f)
        {
            m_animation->Play("standing_right", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
        else
        if(150.0f < rotation && rotation <= 210.0f)
        {
            m_animation->Play("standing_down", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
        else
        if(210.0f < rotation && rotation <= 330.0f)
        {
            m_animation->Play("standing_left", Components::Animation::PlayFlags::Continue | Components::Animation::PlayFlags::Loop);
        }
    }
}
