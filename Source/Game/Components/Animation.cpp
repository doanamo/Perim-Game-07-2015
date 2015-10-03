#include "Precompiled.hpp"
#include "Animation.hpp"
#include "Render.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;
using namespace Components;

Animation::Animation() :
    m_render(nullptr),
    m_currentAnimation(nullptr),
    m_currentFrame(nullptr),
    m_frameIndex(0),
    m_frameTime(0.0f),
    m_playing(false),
    m_loop(false),
    m_update(false)
{
}

Animation::~Animation()
{
}

bool Animation::Finalize(EntityHandle self, const Context& context)
{
    // Get required systems.
    ComponentSystem* componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();
    if(componentSystem == nullptr) return false;

    // Get required components.
    m_render = componentSystem->Lookup<Render>(self);
    if(m_render == nullptr) return false;

    return true;
}

void Animation::SetAnimationList(AnimationListPtr animationList)
{
    this->Stop();

    m_animationList = animationList;
    m_currentAnimation = nullptr;
    m_currentFrame = nullptr;
}

const Animation::AnimationListPtr& Animation::GetAnimationList() const
{
    return m_animationList;
}

void Animation::Update(float timeDelta)
{
    if(m_playing)
    {
        assert(m_currentAnimation != nullptr);
        assert(m_currentFrame != nullptr);

        // Update animation frame.
        while(m_frameTime > m_currentFrame->duration)
        {
            // Substract time of already played frame.
            m_frameTime -= m_currentFrame->duration;
            
            // Advance to the next frame.
            ++m_frameIndex;

            // Check if we are past the last frame.
            if(m_frameIndex >= m_currentAnimation->frames.size())
            {
                if(m_loop)
                {
                    // Go back to the first frame if looped.
                    m_currentFrame = &m_currentAnimation->frames[0];
                    m_frameIndex = 0;
                }
                else
                {
                    // Stop playing the animation.
                    m_playing = false;
                    return;
                }
            }
            else
            {
                // Set the next animation frame.
                m_currentFrame = &m_currentAnimation->frames[m_frameIndex];
            }

            // Set frame change state.
            m_update = true;
        }

        // Set the animation frame sprite.
        if(m_update)
        {
            assert(m_render != nullptr);

            m_render->SetTexture(m_animationList->GetTexture());
            m_render->SetRectangle(m_currentFrame->rectangle);
            m_render->SetOffset(m_currentFrame->offset);

            m_update = false;
        }

        // Increment frame time.
        m_frameTime += timeDelta;
    }
}

void Animation::Play(std::string name, PlayFlags::Type flags)
{
    if(m_animationList == nullptr)
        return;

    // Get the animation.
    const auto* animation = m_animationList->GetAnimation(name);

    if(animation == nullptr)
        return;

    // Check if it's the current animation.
    if(m_currentAnimation == animation)
    {
        // Ignore the request to play the same animation again
        // if we didn't specify that we want to reset it.
        if(!(flags & PlayFlags::Reset))
            return;
    }

    // Play the animation.
    bool continueAnimation = (flags & PlayFlags::Continue) != 0;
    continueAnimation = continueAnimation && m_currentAnimation == animation;
    continueAnimation = continueAnimation && m_frameIndex < m_currentAnimation->frames.size();

    if(continueAnimation)
    {
        // Change animation but preserve the frame index and time.
        m_currentAnimation = animation;
        m_currentFrame = &animation->frames[m_frameIndex];
    }
    else
    {
        // Start new animation.
        m_currentAnimation = animation;
        m_currentFrame = &animation->frames[0];
        m_frameIndex = 0;
        m_frameTime = 0.0f;
    }

    m_loop = flags & PlayFlags::Loop;
    m_playing = true;
    m_update = true;
}

void Animation::Stop()
{
    m_playing = false;
}

bool Animation::IsPlaying() const
{
    return m_playing;
}
