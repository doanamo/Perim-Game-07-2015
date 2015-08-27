#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"
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

    // Component pool interface class.
    class ComponentPoolInterface
    {
    protected:
        ComponentPoolInterface()
        {
        }

    public:
        virtual ~ComponentPoolInterface()
        {
        }

        virtual bool Finalize(EntityHandle handle, const Context& context) = 0;
        virtual bool Remove(EntityHandle handle) = 0;
    };

    // Component pool class.
    template<typename Type>
    class ComponentPool : public ComponentPoolInterface
    {
    public:
        // Check template type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Type declarations.
        typedef std::unordered_map<EntityHandle, Type> ComponentList;
        typedef typename ComponentList::iterator       ComponentIterator;

    public:
        ComponentPool();
        ~ComponentPool();

        // Restores instance to it's original state.
        void Cleanup();

        // Creates a component.
        Type* Create(EntityHandle handle);

        // Lookups a component.
        Type* Lookup(EntityHandle handle);

        // Finalizes a component.
        bool Finalize(EntityHandle handle, const Context& context) override;

        // Removes a component.
        bool Remove(EntityHandle handle) override;

        // Clears all components.
        void Clear();

        // Gets the begin iterator.
        ComponentIterator Begin();

        // Gets the end iterator.
        ComponentIterator End();

    private:
        // List of components.
        ComponentList m_components;
    };

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

    // Inline method definitions.
    template<typename Type>
    ComponentPool<Type>::ComponentPool()
    {
    }

    template<typename Type>
    ComponentPool<Type>::~ComponentPool()
    {
    }

    template<typename Type>
    void ComponentPool<Type>::Cleanup()
    {
        *this = ComponentPool<Type>();
    }

    template<typename Type>
    Type* ComponentPool<Type>::Create(EntityHandle handle)
    {
        // Create a new component for this entity handle.
        auto result = m_components.emplace(std::piecewise_construct, std::forward_as_tuple(handle), std::forward_as_tuple());

        if(result.second == false)
            return nullptr;

        // Return a pointer to a newly created component.
        return &result.first->second;
    }

    template<typename Type>
    Type* ComponentPool<Type>::Lookup(EntityHandle handle)
    {
        // Find a component.
        auto result = m_components.find(handle);

        if(result == m_components.end())
            return nullptr;

        // Make sure handles match.
        BOOST_ASSERT(result->first == handle);

        // Return a pointer to the component.
        return &result->second;
    }

    template<typename Type>
    bool ComponentPool<Type>::Finalize(EntityHandle handle, const Context& context)
    {
        if(m_components.empty())
            return true;

        // Find the component.
        Type* component = this->Lookup(handle);

        // Call the finalizing function.
        if(component != nullptr)
        {
            return component->Finalize(handle, context);
        }
            
        return true;
    }

    template<typename Type>
    bool ComponentPool<Type>::Remove(EntityHandle handle)
    {
        return m_components.erase(handle) == 1;
    }

    template<typename Type>
    void ComponentPool<Type>::Clear()
    {
        m_components.clear();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentPool<Type>::Begin()
    {
        return m_components.begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator ComponentPool<Type>::End()
    {
        return m_components.end();
    }

    template<typename Type>
    void ComponentSystem::Declare()
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

        BOOST_ASSERT(result.second == true);
    }

    template<typename Type>
    Type* ComponentSystem::Create(EntityHandle handle)
    {
        if(!m_initialized)
            return nullptr;

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
        if(!m_initialized)
            return nullptr;

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
        if(!m_initialized)
            return false;

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
        if(!m_initialized)
            return ComponentPool<Type>::ComponentIterator();

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
        if(!m_initialized)
            return ComponentPool<Type>::ComponentIterator();

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
        if(!m_initialized)
            return nullptr;

        // Validate component type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Find pool by component type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
            return nullptr;

        // Cast and return the pointer that we already know is a component pool.
        return reinterpret_cast<ComponentPool<Type>*>(it->second.get());
    }
}
