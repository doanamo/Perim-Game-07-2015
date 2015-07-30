#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Identity System
//

namespace Game
{
    // Forward declarations.
    namespace Event
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

        // Restores class instance to it's original state.
        void Cleanup();

        // Initializes the identity system.
        bool Initialize();

        // Sets entity name.
        bool SetEntityName(const EntityHandle& entity, std::string name);

        // Gets entity name.
        std::string GetEntityName(const EntityHandle& entity) const;

        // Lookups an entity by name.
        EntityHandle Lookup(std::string name) const;

    public:
        // Connects to a signal.
        void ConnectSignal(boost::signals2::signal<void(const Event::EntityDestroyed&)>& signal);

    private:
        // Called when an entity gets destroyed.
        void OnEntityDestroyed(const Event::EntityDestroyed& event);

    private:
        // System state.
        bool m_initialized;

        // List of named entities.
        EntityNameList m_names;

        // Signal connections.
        boost::signals2::connection m_entityDestroyed;
    };
}
