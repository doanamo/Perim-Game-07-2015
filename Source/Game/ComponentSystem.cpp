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
    // Bind event receivers.
    m_entityFinalize.Bind<ComponentSystem, &ComponentSystem::OnEntityFinalize>(this);
    m_entityDestroyed.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroyed>(this);
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

    // Unsubscribe event signals.
    m_entityFinalize.Unsubscribe();
    m_entityDestroyed.Unsubscribe();

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

void ComponentSystem::Subscribe(DispatcherBase<bool(const Game::Events::EntityFinalize&)>& dispatcher)
{
    if(!m_initialized)
        return;

    dispatcher.Subscribe(m_entityFinalize);
}

void ComponentSystem::Subscribe(DispatcherBase<void(const Game::Events::EntityDestroyed&)>& dispatcher)
{
    if(!m_initialized)
        return;

    dispatcher.Subscribe(m_entityDestroyed);
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
