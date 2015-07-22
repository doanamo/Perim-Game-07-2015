#pragma once

#include "Precompiled.hpp"
#include "ComponentPool.hpp"

//
// Component System
//

namespace Game
{    
    // Component system class.
    class ComponentSystem
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ComponentPoolInterface>               ComponentPoolPtr;
        typedef std::unordered_map<std::type_index, ComponentPoolPtr> ComponentPoolList;
        typedef ComponentPoolList::value_type                         ComponentPoolPair;

    public:
        ComponentSystem() :
            m_initialized(false)
        {
        }

        ~ComponentSystem()
        {
            Cleanup();
        }

        // Restores class instance to it's original state.
        void Cleanup()
        {
            m_entityDestroyed.disconnect();

            Utility::ClearContainer(m_pools);

            m_initialized = false;
        }

        // Initializes the component system.
        bool Initialize()
        {
            Cleanup();

            // Success!
            return m_initialized = true;
        }

        // Declares a component type.
        template<typename Type>
        void Declare()
        {
            if(!m_initialized)
                return;

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

            assert(result.second == true);
        }

        // Creates a component.
        template<typename Type>
        Type* Create(EntityHandle handle)
        {
            if(!m_initialized)
                return nullptr;

            // Validate component type.
            static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

            // Get the component pool.
            ComponentPool<Type>* pool = GetComponentPool<Type>();

            if(pool == nullptr)
                return nullptr;

            // Create and return the component.
            return pool->Create(handle);
        }

        // Lookups a component.
        template<typename Type>
        Type* Lookup(EntityHandle handle)
        {
            if(!m_initialized)
                return nullptr;

            // Validate component type.
            static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

            // Get the component pool.
            ComponentPool<Type>* pool = GetComponentPool<Type>();

            if(pool == nullptr)
                return nullptr;

            // Lookup and return the component.
            return pool->Lookup(handle);
        }

        // Removes a component.
        template<typename Type>
        bool Remove(EntityHandle handle)
        {
            if(!m_initialized)
                return;

            // Validate component type.
            static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

            // Get the component pool.
            ComponentPool<Type>* pool = GetComponentPool<Type>();

            if(pool == nullptr)
                return false;

            // Remove a component.
            return pool->Remove(handle);
        }

        // Gets the begin iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator Begin()
        {
            if(!m_initialized)
                return ComponentPool<Type>::ComponentIterator();

            // Validate component type.
            static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

            // Get the component pool.
            ComponentPool<Type>* pool = GetComponentPool<Type>();

            if(pool == nullptr)
                return ComponentPool<Type>::ComponentIterator();

            // Return the iterator.
            return pool->Begin();
        }

        // Gets the end iterator.
        template<typename Type>
        typename ComponentPool<Type>::ComponentIterator End()
        {
            if(!m_initialized)
                return ComponentPool<Type>::ComponentIterator();

            // Validate component type.
            static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

            // Get the component pool.
            ComponentPool<Type>* pool = GetComponentPool<Type>();

            if(pool == nullptr)
                return ComponentPool<Type>::ComponentIterator();

            // Return the iterator.
            return pool->End();
        }

        // Gets a component pool.
        template<typename Type>
        ComponentPool<Type>* GetComponentPool()
        {
            if(!m_initialized)
                return nullptr;

            // Validate component type.
            static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

            // Find pool by component type.
            auto it = m_pools.find(typeid(Type));

            if(it == m_pools.end())
                return nullptr;

            // Cast the pointer that we already know is a component pool.
            ComponentPool<Type>* pool = reinterpret_cast<ComponentPool<Type>*>(it->second.get());

            // Return the pool.
            return pool;
        }

    public:
        // Connects to entity destroyed event signal.
        void ConnectEntityDestroyed(boost::signals2::signal<void(EntityHandle)>& signal)
        {
            m_entityDestroyed = signal.connect(boost::bind(&ComponentSystem::OnEntityDestroyed, this, _1));
        }

    private:
        // Called when an entity gets destroyed.
        void OnEntityDestroyed(EntityHandle handle)
        {
            if(!m_initialized)
                return;

            // Remove entity components from every pool.
            for(auto& pair : m_pools)
            {
                auto& pool = pair.second;
                pool->Remove(handle);
            }
        }

    private:
        // System state.
        bool m_initialized;

        // Component pools.
        ComponentPoolList m_pools;

        // Signal connections.
        boost::signals2::connection m_entityDestroyed;
    };
}
