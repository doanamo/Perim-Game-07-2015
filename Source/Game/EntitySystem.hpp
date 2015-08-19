#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Entity Events
//

namespace Game
{
    namespace Events
    {
        // Entity finalize event structure.
        struct EntityFinalize
        {
            EntityFinalize(EntityHandle handle) :
                handle(handle)
            {
            }

            EntityHandle handle;
        };

        // Entity created event structure.
        struct EntityCreated
        {
            EntityCreated(EntityHandle handle) :
                handle(handle)
            {
            }

            EntityHandle handle;
        };

        // Entity destroyed event structure.
        struct EntityDestroyed
        {
            EntityDestroyed(EntityHandle handle) :
                handle(handle)
            {
            }

            EntityHandle handle;
        };
    }
}

//
// Entity System
//

namespace Game
{
    // Entity system class.
    class EntitySystem
    {
    public:
        // Handle flags.
        struct HandleFlags
        {
            enum Type
            {
                // Entity handle has been allocated but is not being used.
                None = 0,

                // Entity handle has been created and is valid.
                Valid = 1 << 0,

                // Entity handle has been created and is active.
                Active = 1 << 1,

                // Entity handle has been scheduled to be destroyed.
                Destroy = 1 << 2,
            };

            static const uint32_t Free = None;
        };

        // Handle entry structure.
        struct HandleEntry
        {
            EntityHandle handle;
            int          nextFree;
            uint32_t     flags;
        };

        // Entity command types.
        struct EntityCommands
        {
            enum Type
            {
                Invalid,

                Create,
                Destroy,
            };
        };

        // Entity command structure.
        struct EntityCommand
        {
            EntityCommands::Type type;
            EntityHandle handle;
        };

    private:
        // Signal combiner.
        class FailureAbort
        {
        public:
            typedef bool result_type;

            template<typename Type>
            result_type operator()(Type begin, Type end) const
            {
                // Abort when first listener return false.
                for(auto it = begin; it != end; ++it)
                {
                    if(*it == false)
                        return false;
                }

                return true;
            }
        };

        // Type declarations.
        typedef std::vector<HandleEntry>   HandleList;
        typedef std::vector<EntityCommand> CommandList;

    public:
        // Signal type declarations.
        typedef boost::signals2::signal<bool(const Events::EntityFinalize&), FailureAbort> EntityFinalizeSignal;
        typedef boost::signals2::signal<void(const Events::EntityCreated&)> EntityCreatedSignal;
        typedef boost::signals2::signal<void(const Events::EntityDestroyed&)> EntityDestroyedSignal;

    public:
        EntitySystem();
        ~EntitySystem();

        // Initializes the entity system.
        bool Initialize(Context& context);

        // Creates an entity.
        EntityHandle CreateEntity();

        // Destroys an entity.
        void DestroyEntity(const EntityHandle& entity);

        // Destroys all entities.
        void DestroyAllEntities();

        // Process entity commands.
        void ProcessCommands();

        // Checks if an entity handle is valid.
        bool IsHandleValid(const EntityHandle& entity) const;

        // Returns the number of active entities.
        unsigned int GetEntityCount() const;

    private:
        // Frees an entity handle.
        void FreeHandle(int handleIndex, HandleEntry& handleEntry);

    public:
        // Event signals.
        EntityFinalizeSignal  entityFinalize;
        EntityCreatedSignal   entityCreated;
        EntityDestroyedSignal entityDestroyed;

    private:
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        HandleList m_handles;

        // Number of active entities.
        unsigned int m_entityCount;

        // List of free handles.
        int  m_freeListDequeue;
        int  m_freeListEnqueue;
        bool m_freeListIsEmpty;

        // Initialization state.
        bool m_initialized;
    };
}
