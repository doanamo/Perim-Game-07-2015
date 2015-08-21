#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "EntitySystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Script.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the script system! "
}

ScriptSystem::ScriptSystem() :
    m_entitySystem(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

ScriptSystem::~ScriptSystem()
{
    if(m_initialized)
        this->Cleanup();
}

void ScriptSystem::Cleanup()
{
    // Reset context references.
    m_entitySystem = nullptr;
    m_componentSystem = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool ScriptSystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

    // Add system to the context.
    if(context[ContextTypes::Game].Has<ScriptSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Game].Set(this);

    // Get the entity system.
    m_entitySystem = context[ContextTypes::Game].Get<EntitySystem>();

    if(m_entitySystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing EntitySystem instance.";
        return false;
    }

    // Get the component system.
    m_componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();

    if(m_componentSystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing ComponentSystem instance.";
        return false;
    }

    // Declare required components.
    m_componentSystem->Declare<Game::Components::Script>();

    // Success!
    return m_initialized = true;
}

void ScriptSystem::Update(float timeDelta)
{
    if(!m_initialized)
        return;

    // Iterate over all script components.
    auto componentsBegin = m_componentSystem->Begin<Components::Script>();
    auto componentsEnd = m_componentSystem->End<Components::Script>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        const EntityHandle& entity = it->first;
        Components::Script& script = it->second;

        // Check if entity is active.
        if(!m_entitySystem->IsHandleValid(it->first))
            continue;

        // Update script component.
        script.Update(entity, timeDelta);
    }
}
