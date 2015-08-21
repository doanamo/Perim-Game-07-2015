#pragma once

#include "Precompiled.hpp"
#include "ComponentPool.hpp"
#include "EntitySystem.hpp"

//
// Component System
//

namespace Game
{    
    // Forward declarations.
    namespace Events
    {
        struct EntityFinalize;
        struct EntityDestroyed;
    }

    // Component system class.
    class ComponentSystem
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ComponentPoolInterface>               ComponentPoolPtr;
        typedef std::unordered_map<std::type_index, ComponentPoolPtr> ComponentPoolList;
        typedef ComponentPoolList::value_type                         ComponentPoolPair;

    public:
        ComponentSystem();
        ~ComponentSystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the component system.
        bool Initialize(Context& context);

        // Declares a component type.
        template<typename Type>
        void Declare();

        // Creates a component.
        template<typename Type>
        Type* Create(EntityHandle handle);

        // Lookups a component.
        template<typename Type>
        Type* Lookup(EntityHandle handle);

        // Removes a component.
        template<typename Type>
        bool Remove(EntityHandle handle);

        // Gets the begin iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator Begin();

        // Gets the end iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator End();

        // Gets a component pool.
        template<typename Type>
        ComponentPool<Type>* GetPool();

        // Connects to a signal.
        void ConnectSignal(EntitySystem::EntityFinalizeSignal& signal);
        void ConnectSignal(EntitySystem::EntityDestroyedSignal& signal);

    private:
        // Called when an entity needs to be finalized.
        bool OnEntityFinalize(const Events::EntityFinalize& event);

        // Called when an entity gets destroyed.
        void OnEntityDestroyed(const Events::EntityDestroyed& event);

    private:
        // Component pools.
        ComponentPoolList m_pools;

        // Signal connections.
        boost::signals2::scoped_connection m_entityFinalize;
        boost::signals2::scoped_connection m_entityDestroyed;

        // Context reference.
        Context* m_context;

        // Initialization state.
        bool m_initialized;
    };

    // Inline method bodies.
    template<typename Type>
    void ComponentSystem::Declare()
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Check if component type was already declared.
        auto it = m_pools.find(typeid(Type));

        if(it != m_pools.end())
            return;

        // Create and add pool to the collection.
        auto pool = std::make_unique<ComponentPool<Type>>();
        auto pair = ComponentPoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        BOOST_ASSERT(result.second == true);
    }

    template<typename Type>
    Type* ComponentSystem::Create(EntityHandle handle)
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();

        if(pool == nullptr)
            return nullptr;

        // Create and return the component.
        return pool->Create(handle);
    }

    template<typename Type>
    Type* ComponentSystem::Lookup(EntityHandle handle)
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();

        if(pool == nullptr)
            return nullptr;

        // Lookup and return the component.
        return pool->Lookup(handle);
    }

    template<typename Type>
    bool ComponentSystem::Remove(EntityHandle handle)
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();

        if(pool == nullptr)
            return false;

        // Remove a component.
        return pool->Remove(handle);
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentSystem::Begin()
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();

        if(pool == nullptr)
            return ComponentPool<Type>::ComponentIterator();

        // Return the iterator.
        return pool->Begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentSystem::End()
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = this->GetPool<Type>();

        if(pool == nullptr)
            return ComponentPool<Type>::ComponentIterator();

        // Return the iterator.
        return pool->End();
    }

    template<typename Type>
    ComponentPool<Type>* ComponentSystem::GetPool()
    {
        BOOST_ASSERT(m_initialized);

        // Validate component type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Find pool by component type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
            return nullptr;

        // Cast the pointer that we already know is a component pool.
        ComponentPool<Type>* pool = reinterpret_cast<ComponentPool<Type>*>(it->second.get());

        // Return the pool.
        return pool;
    }
}
