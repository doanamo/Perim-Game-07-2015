#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
using namespace Game;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the component system! "
}

ComponentSystem::ComponentSystem() :
    m_context(nullptr),
    m_initialized(false)
{
}

ComponentSystem::~ComponentSystem()
{
    if(m_initialized)
        this->Cleanup();
}

void ComponentSystem::Cleanup()
{
    // Clear all component pools.
    Utility::ClearContainer(m_pools);

    // Disconnect event signals.
    m_entityFinalize.disconnect();
    m_entityDestroyed.disconnect();

    // Reset context reference.
    m_context = nullptr;

    // Reset initialization state.
    m_initialized = false;
}

bool ComponentSystem::Initialize(Context& context)
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
    if(context[ContextTypes::Game].Has<ComponentSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Game].Set(this);

    // Save context reference.
    m_context = &context;

    // Success!
    return m_initialized = true;
}

void ComponentSystem::ConnectSignal(EntitySystem::EntityFinalizeSignal& signal)
{
    if(!m_initialized)
        return;

    m_entityFinalize = signal.connect(boost::bind(&ComponentSystem::OnEntityFinalize, this, _1));
}

void ComponentSystem::ConnectSignal(EntitySystem::EntityDestroyedSignal& signal)
{
    if(!m_initialized)
        return;

    m_entityDestroyed = signal.connect(boost::bind(&ComponentSystem::OnEntityDestroyed, this, _1));
}

bool ComponentSystem::OnEntityFinalize(const Events::EntityFinalize& event)
{
    assert(m_initialized);
    assert(m_context != nullptr);

    // Finalize entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;

        if(!pool->Finalize(event.handle, *m_context))
            return false;
    }

    return true;
}

void ComponentSystem::OnEntityDestroyed(const Events::EntityDestroyed& event)
{
    assert(m_initialized);

    // Remove entity components from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->Remove(event.handle);
    }
}
