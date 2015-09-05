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
    // Bind event receivers.
    m_entityDestroyed.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyed>(this);
}

IdentitySystem::~IdentitySystem()
{
    if(m_initialized)
        this->Cleanup();
}

void IdentitySystem::Cleanup()
{
    // Clear named entities lists.
    Utility::ClearContainer(m_storage);
    Utility::ClearContainer(m_entities);
    Utility::ClearContainer(m_names);

    // Unsubscribe receivers.
    m_entityDestroyed.Unsubscribe();

    // Reset initialization state.
    m_initialized = false;
}

bool IdentitySystem::Initialize(Context& context)
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

    // Subscribe receivers.
    entitySystem->events.entityDestroyed.Subscribe(m_entityDestroyed);

    // Success!
    return m_initialized = true;
}

bool IdentitySystem::SetEntityName(const EntityHandle& entity, std::string name)
{
    if(!m_initialized)
        return false;

    // Find out if we already have this entity named.
    auto it = m_entities.find(&entity);

    if(it != m_entities.end())
    {
        std::size_t index = it->second;

        // If name is not empty, rename entity.
        // If name is empty, remove entity from registry.
        if(!name.empty())
        {
            // Replace entity name.
            m_storage[index].second = name;
        }
        else
        {
            // Remove named entity.
            this->RemoveElement(index);
        }
    }
    else
    {
        // Create new element in the storage.
        m_storage.emplace_back(entity, name);

        // Create lookup references.
        m_entities.emplace(&m_storage.back().first, m_storage.size() - 1);
        m_names.emplace(&m_storage.back().second, m_storage.size() - 1);
    }

    return true;
}

std::string IdentitySystem::GetEntityName(const EntityHandle& entity) const
{
    if(!m_initialized)
        return InvalidName;

    // Find the entity.
    auto it = m_entities.find(&entity);

    if(it != m_entities.end())
    {
        // Return entity name.
        std::size_t index = it->second;
        return m_storage[index].second;
    }
    else
    {
        // Entity is annonymous.
        return InvalidName;
    }
}

EntityHandle IdentitySystem::Lookup(std::string name) const
{
    if(!m_initialized)
        return EntityHandle();

    // Find the name.
    auto it = m_names.find(&name);

    if(it != m_names.end())
    {
        // Return named entity.
        std::size_t index = it->second;
        return m_storage[index].first;
    }
    else
    {
        // Return an invalid entity.
        return EntityHandle();
    }
}

void IdentitySystem::RemoveElement(std::size_t index)
{
    assert(m_initialized);

    // Move storage element to the end of the queue before erasing it to 
    // avoid invalidating pointers/indices referencing the container.
    m_entities[&m_storage.back().first] = index;
    m_names[&m_storage.back().second] = index;

    std::swap(m_storage[index], m_storage.back());

    // Remove the element we moved that's at the back now.
    m_entities.erase(&m_storage.back().first);
    m_names.erase(&m_storage.back().second);

    m_storage.pop_back();
}

void IdentitySystem::OnEntityDestroyed(const Events::EntityDestroyed& event)
{
    assert(m_initialized);

    // Remove entity if it was registered.
    auto it = m_entities.find(&event.handle);
    
    if(it != m_entities.end())
    {
        std::size_t index = it->second;
        this->RemoveElement(index);
    }
}
