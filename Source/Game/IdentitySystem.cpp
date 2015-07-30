#include "Precompiled.hpp"
#include "IdentitySystem.hpp"
#include "EntitySystem.hpp"
using namespace Game;

namespace
{
    // Invalid name constant.
    const char* InvalidName = "";
}

IdentitySystem::IdentitySystem() :
    m_initialized(false)
{
}

IdentitySystem::~IdentitySystem()
{
    Cleanup();
}

void IdentitySystem::Cleanup()
{
    m_entityDestroyed.disconnect();

    Utility::ClearContainer(m_names);

    m_initialized = false;
}

bool IdentitySystem::Initialize()
{
    Cleanup();

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

void IdentitySystem::ConnectSignal(boost::signals2::signal<void(const Event::EntityDestroyed&)>& signal)
{
    m_entityDestroyed = signal.connect(boost::bind(&IdentitySystem::OnEntityDestroyed, this, _1));
}

void IdentitySystem::OnEntityDestroyed(const Event::EntityDestroyed& event)
{
    if(!m_initialized)
        return;

    // Remove entity from the name map.
    auto result = m_names.right.erase(event.handle);
    assert(result == 0 || result == 1);
}
