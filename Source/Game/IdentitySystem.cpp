#include "Precompiled.hpp"
#include "IdentitySystem.hpp"
#include "EntitySystem.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the identity system! "

    // Invalid name constant.
    const char* InvalidName = "";
}

IdentitySystem::IdentitySystem() :
    m_initialized(false)
{
}

IdentitySystem::~IdentitySystem()
{
    if(m_initialized)
        this->Cleanup();
}

void IdentitySystem::Cleanup()
{
    // Clear the container.
    Utility::ClearContainer(m_names);

    // Disconnect signals.
    m_entityDestroyed.disconnect();

    // Reset initialization state.
    m_initialized = false;
}

bool IdentitySystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

    // Add instance to the context.
    if(context[ContextTypes::Game].Has<IdentitySystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Game].Set(this);

    // Get the entity system.
    EntitySystem* entitySystem = context[ContextTypes::Game].Get<EntitySystem>();

    if(entitySystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing EntitySystem instance.";
        return false;
    }

    // Connect to an event signal.
    m_entityDestroyed = entitySystem->events.entityDestroyed.connect(
        boost::bind(&IdentitySystem::OnEntityDestroyed, this, _1));

    // Success!
    return m_initialized = true;
}

bool IdentitySystem::SetEntityName(const EntityHandle& entity, std::string name)
{
    if(!m_initialized)
        return false;

    // Remove existing entity from the map (make it anonymous). 
    auto it = m_names.right.find(entity);

    if(it != m_names.right.end())
    {
        // Check if it already has the same name.
        if(it->second == name)
            return true;

        // Remove entity from the map.
        m_names.right.erase(it);
    }

    // Insert entity with a new name.
    if(!name.empty())
    {
        auto result = m_names.left.insert(std::make_pair(name, entity));
        return result.second;
    }

    return true;
}

std::string IdentitySystem::GetEntityName(const EntityHandle& entity) const
{
    if(!m_initialized)
        return InvalidName;

    // Find entity name.
    auto it = m_names.right.find(entity);

    if(it != m_names.right.end())
    {
        // Return entity name.
        return it->second;
    }
    else
    {
        // Entity doesn't have a name.
        return InvalidName;
    }
}

EntityHandle IdentitySystem::Lookup(std::string name) const
{
    if(!m_initialized)
        return EntityHandle();

    // Find entity by name.
    auto it = m_names.left.find(name);

    if(it != m_names.left.end())
    {
        // Return entity refered by this name.
        return it->second;
    }
    else
    {
        // Return an invalid entity.
        return EntityHandle();
    }
}

void IdentitySystem::OnEntityDestroyed(const Events::EntityDestroyed& event)
{
    BOOST_ASSERT(m_initialized);

    // Remove entity from the name map.
    auto result = m_names.right.erase(event.handle);
    assert(result == 0 || result == 1);
}
