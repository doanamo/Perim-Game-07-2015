#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "EntitySystem.hpp"
#include "ComponentSystem.hpp"
#include "Components/Script.hpp"
using namespace Game;

ScriptSystem::ScriptSystem() :
    m_entitySystem(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

ScriptSystem::~ScriptSystem()
{
}

bool ScriptSystem::Initialize(Context& context)
{
    BOOST_ASSERT(!m_initialized);

    // Add system to the context.
    BOOST_ASSERT(context[ContextTypes::Game].Set(this));

    // Get required systems.
    m_entitySystem = context[ContextTypes::Game].Get<EntitySystem>();
    BOOST_ASSERT_MSG(m_entitySystem != nullptr, "Context is missing EntitySystem instance.");

    m_componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();
    BOOST_ASSERT_MSG(m_componentSystem != nullptr, "Context is missing ComponentSystem instance.");

    // Declare required components.
    m_componentSystem->Declare<Game::Components::Script>();

    // Success!
    return m_initialized = true;
}

void ScriptSystem::Update(float timeDelta)
{
    BOOST_ASSERT(m_initialized);

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
