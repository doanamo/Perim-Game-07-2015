#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Identity System
//

namespace Game
{
    // Forward declarations.
    namespace Events
    {
        struct EntityDestroyed;
    }

    // Identity system class.
    class IdentitySystem
    {
    private:
        // Comparison functor.
        template<typename Type>
        struct CompareDerefernced
        {
            bool operator()(const Type* left, const Type* right)
            {
                return *left < *right;
            }
        };

        // Type definitions.
        typedef std::pair<EntityHandle, std::string> EntityNamePair;
        typedef std::deque<EntityNamePair> EntityNameStorage;

        typedef std::map<const EntityHandle*, std::size_t, CompareDerefernced<EntityHandle>> EntityLookupList;
        typedef std::map<const std::string*, std::size_t, CompareDerefernced<std::string>> NameLookupList;

    public:
        IdentitySystem();
        ~IdentitySystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the identity system instance.
        bool Initialize(Context& context);

        // Sets entity name.
        bool SetEntityName(const EntityHandle& entity, std::string name);

        // Gets entity name.
        std::string GetEntityName(const EntityHandle& entity) const;

        // Lookups an entity by name.
        EntityHandle Lookup(std::string name) const;

    private:
        // Removes an element by index.
        void RemoveElement(std::size_t index);

        // Called when an entity gets destroyed.
        void OnEntityDestroyed(const Events::EntityDestroyed& event);

    private:
        // Registry of named entities.
        EntityNameStorage m_storage;
        EntityLookupList m_entities;
        NameLookupList m_names;

        // Signal connections.
        boost::signals2::scoped_connection m_entityDestroyed;

        // Initialization state.
        bool m_initialized;
    };
}
