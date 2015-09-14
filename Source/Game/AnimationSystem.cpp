#include "Precompiled.hpp"
#include "AnimationSystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Animation.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the animation system! "
}

AnimationSystem::AnimationSystem() :
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

AnimationSystem::~AnimationSystem()
{
    if(m_initialized)
        this->Cleanup();
}

void AnimationSystem::Cleanup()
{
    // Reset context references.
    m_componentSystem = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool AnimationSystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Add instance to the context.
    if(context[ContextTypes::Game].Has<AnimationSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Game].Set(this);

    // Get the component system.
    m_componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();

    if(m_componentSystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing ComponentSystem instance.";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void AnimationSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Iterate over all animation components.
    auto componentsBegin = m_componentSystem->Begin<Components::Animation>();
    auto componentsEnd = m_componentSystem->End<Components::Animation>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        Components::Animation& animation = it->second;

        // Update animation component.
        animation.Update(timeDelta);
    }
}
