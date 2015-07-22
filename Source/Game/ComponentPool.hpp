#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"
#include "EntityHandle.hpp"

//
// Component Pool Interface
//

namespace Game
{
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

        virtual void Remove(EntityHandle handle) = 0;
    };
}

//
// Component Pool
//

namespace Game
{
    // Component pool class.
    template<typename Type>
    class ComponentPool : public ComponentPoolInterface
    {
    public:
        // Check template type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Type declarations.
        typedef std::unordered_map<EntityHandle, Type> ComponentList;
        typedef typename ComponentList::iterator       ComponentIterator;

    public:
        ComponentPool()
        {
        }

        ~ComponentPool()
        {
            Cleanup();
        }

        // Restores class instance to it's original state.
        void Cleanup()
        {
            // Free component memory.
            Utility::ClearContainer(m_components);
        }

        // Creates a component.
        Type* Create(EntityHandle handle)
        {
            // Create a new component for this entity handle.
            auto result = m_components.emplace(std::make_pair(handle, Type()));

            if(result.second == false)
                return nullptr;

            // Return a pointer to a newly created component.
            return &result.first->second;
        }

        // Lookups a component.
        Type* Lookup(EntityHandle handle)
        {
            // Find a component.
            auto result = m_components.find(handle);

            if(result == m_components.end())
                return nullptr;

            // Make sure handles match.
            assert(result->first == handle);

            // Return a pointer to the component.
            return &result->second;
        }

        // Removes a component.
        bool Remove(EntityHandle handle)
        {
            return m_components.erase(handle) == 1;
        }

        // Clears all components.
        void Clear()
        {
            m_components.clear();
        }

        // Gets the begin iterator.
        ComponentIterator Begin()
        {
            return m_components.begin();
        }

        // Gets the end iterator.
        ComponentIterator End()
        {
            return m_components.end();
        }

    private:
        // List of components.
        ComponentList m_components;
    };
}