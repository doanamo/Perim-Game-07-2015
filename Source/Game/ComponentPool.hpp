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

        virtual bool Finalize(EntityHandle handle, const Context& context) = 0;
        virtual bool Remove(EntityHandle handle) = 0;
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
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Type declarations.
        typedef std::unordered_map<EntityHandle, Type> ComponentList;
        typedef typename ComponentList::iterator       ComponentIterator;

    public:
        ComponentPool()
        {
        }

        ~ComponentPool()
        {
        }

        // Creates a component.
        Type* Create(EntityHandle handle)
        {
            // Create a new component for this entity handle.
            auto result = m_components.emplace(std::piecewise_construct, std::forward_as_tuple(handle), std::forward_as_tuple());

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
            BOOST_ASSERT(result->first == handle);

            // Return a pointer to the component.
            return &result->second;
        }

        // Finalizes a component.
        bool Finalize(EntityHandle handle, const Context& context) override
        {
            // Find the component.
            Type* component = this->Lookup(handle);
            BOOST_ASSERT(component != nullptr);

            // Call the finalizing function.
            return component->Finalize(handle, context);
        }

        // Removes a component.
        bool Remove(EntityHandle handle) override
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
