#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem() :
    m_initialized(false)
{
}

ComponentSystem::~ComponentSystem()
{
}

bool ComponentSystem::Initialize(Context& context)
{
    BOOST_ASSERT(!m_initialized);

    // Add system to the context.
    BOOST_ASSERT(context.Set(this));

    // Success!
    return m_initialized = true;
}

void ComponentSystem::ConnectSignal(EntitySystem::EntityFinalizeSignal& signal)
{
    BOOST_ASSERT(m_initialized);

    m_entityFinalize = signal.connect(boost::bind(&ComponentSystem::OnEntityFinalize, this, _1));
}

void ComponentSystem::ConnectSignal(EntitySystem::EntityDestroyedSignal& signal)
{
    BOOST_ASSERT(m_initialized);

    m_entityDestroyed = signal.connect(boost::bind(&ComponentSystem::OnEntityDestroyed, this, _1));
}

bool ComponentSystem::OnEntityFinalize(const Events::EntityFinalize& event)
{
    BOOST_ASSERT(m_initialized);

    // Finalize entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;

        if(!pool->Finalize(event.handle))
            return false;
    }

    return true;
}

void ComponentSystem::OnEntityDestroyed(const Events::EntityDestroyed& event)
{
    BOOST_ASSERT(m_initialized);

    // Remove entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->Remove(event.handle);
    }
}
