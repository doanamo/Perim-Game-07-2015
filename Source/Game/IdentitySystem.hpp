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
        // Type definitions.
        typedef boost::bimap<
            boost::bimaps::unordered_set_of<std::string>,
            boost::bimaps::unordered_set_of<EntityHandle>
        > EntityNameList;

    public:
        IdentitySystem();
        ~IdentitySystem();

        // Initializes the identity system.
        bool Initialize(Context& context);

        // Sets entity name.
        bool SetEntityName(const EntityHandle& entity, std::string name);

        // Gets entity name.
        std::string GetEntityName(const EntityHandle& entity) const;

        // Lookups an entity by name.
        EntityHandle Lookup(std::string name) const;

    private:
        // Called when an entity gets destroyed.
        void OnEntityDestroyed(const Events::EntityDestroyed& event);

    private:
        // List of named entities.
        EntityNameList m_names;

        // Signal connections.
        boost::signals2::scoped_connection m_entityDestroyed;

        // Initialization state.
        bool m_initialized;
    };
}
