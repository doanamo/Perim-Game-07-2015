#include "Precompiled.hpp"
#include "EntitySystem.hpp"
using namespace Game;

namespace
{
    // Constant variables.
    const int MaximumIdentifier   = std::numeric_limits<int>::max();
    const int InvalidIdentifier   = 0;
    const int InvalidNextFree     = -1;
    const int InvalidQueueElement = -1;
}

EntitySystem::EntitySystem() :
    m_initialized(false),
    m_entityCount(0),
    m_freeListDequeue(InvalidQueueElement),
    m_freeListEnqueue(InvalidQueueElement),
    m_freeListIsEmpty(true)
{
}

EntitySystem::~EntitySystem()
{
    Cleanup();
}

void EntitySystem::Cleanup()
{
    // Process remaining commands.
    ProcessCommands();

    Utility::ClearContainer(m_commands);

    // Destroy all entities.
    DestroyAllEntities();

    Utility::ClearContainer(m_handles);

    // Reset the entity counter.
    m_entityCount = 0;

    // Reset the free list queue.
    m_freeListDequeue = InvalidQueueElement;
    m_freeListEnqueue = InvalidQueueElement;
    m_freeListIsEmpty = true;

    // System state.
    m_initialized = false;
}

bool EntitySystem::Initialize()
{
    Cleanup();

    // Success!
    return m_initialized = true;
}

EntityHandle EntitySystem::CreateEntity()
{
    BOOST_ASSERT(m_initialized);

    // Check if we reached the numerical limits.
    BOOST_ASSERT(m_handles.size() != MaximumIdentifier);

    // Create a new handle if the free list queue is empty.
    if(m_freeListIsEmpty)
    {
        // Create an entity handle.
        EntityHandle handle;
        handle.identifier = m_handles.size() + 1;
        handle.version = 0;

        // Create a handle entry.
        HandleEntry entry;
        entry.handle = handle;
        entry.nextFree = InvalidNextFree;
        entry.flags = HandleFlags::Free;

        m_handles.push_back(entry);

        // Add new handle entry to the free list queue.
        int handleIndex = m_handles.size() - 1;

        m_freeListDequeue = handleIndex;
        m_freeListEnqueue = handleIndex;
        m_freeListIsEmpty = false;
    }

    // Retrieve an unused handle from the free list.
    int handleIndex = m_freeListDequeue;
    HandleEntry& handleEntry = m_handles[m_freeListDequeue];

    // Clear next free handle index.
    handleEntry.nextFree = InvalidNextFree;

    // Update the free list queue.
    if(m_freeListDequeue == m_freeListEnqueue)
    {
        // If there was only one element in the queue,
        // set the free list queue state to empty.
        m_freeListDequeue = InvalidQueueElement;
        m_freeListEnqueue = InvalidQueueElement;
        m_freeListIsEmpty = true;
    }
    else
    {
        // If there were more than a single element in the queue,
        // set the beginning of the queue to the next free element.
        m_freeListDequeue = handleEntry.nextFree;
    }

    // Mark handle as valid.
    handleEntry.flags |= HandleFlags::Valid;

    // Add a create entity command.
    EntityCommand command;
    command.type = EntityCommands::Create;
    command.handle = handleEntry.handle;

    m_commands.push_back(command);

    // Return the handle, which is still inactive
    // until the next ProcessCommands() call.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entity)
{
    BOOST_ASSERT(m_initialized);

    // Check if the handle is valid.
    if(!IsHandleValid(entity))
        return;

    // Locate the handle entry.
    int handleIndex = entity.identifier - 1;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Set the handle destroy flag.
    handleEntry.flags |= HandleFlags::Destroy;

    // Add a destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handle = handleEntry.handle;

    m_commands.push_back(command);
}

void EntitySystem::DestroyAllEntities()
{
    if(!m_initialized)
        return;

    // Process entity commands.
    ProcessCommands();

    // Check if there are any entities to destroy.
    if(m_handles.empty())
        return;

    // Inform about entities soon to be destroyed.
    for(auto it = m_handles.begin(); it != m_handles.end(); ++it)
    {
        HandleEntry& handleEntry = *it;

        if(handleEntry.flags & HandleFlags::Valid)
        {
            // Send event about soon to be destroyed entity.
            this->entityDestroyed(handleEntry.handle);

            // Set the handle free flags.
            handleEntry.flags = HandleFlags::Free;

            // Increment the handle version to invalidate it.
            handleEntry.handle.version += 1;
        }
    }

    // Chain handles to form a free list.
    for(unsigned int i = 0; i < m_handles.size(); ++i)
    {
        HandleEntry& handleEntry = m_handles[i];
        handleEntry.nextFree = i + 1;
    }

    // Close the free list queue chain at the end.
    int lastHandleIndex = m_handles.size() - 1;
    m_handles[lastHandleIndex].nextFree = InvalidNextFree;

    // Set the free list variables.
    m_freeListDequeue = 0;
    m_freeListEnqueue = lastHandleIndex;
    m_freeListIsEmpty = false;
}

void EntitySystem::ProcessCommands()
{
    if(!m_initialized)
        return;

    // Process entity commands.
    for(auto command = m_commands.begin(); command != m_commands.end(); ++command)
    {
        switch(command->type)
        {
        case EntityCommands::Create:
            {
                // Locate the handle entry.
                int handleIndex = command->handle.identifier - 1;
                HandleEntry& handleEntry = m_handles[handleIndex];

                // Make sure handles match.
                BOOST_ASSERT(command->handle == handleEntry.handle);

                // Mark handle as active.
                BOOST_ASSERT(!(handleEntry.flags & HandleFlags::Active));

                handleEntry.flags |= HandleFlags::Active;

                // Increment the counter of active entities.
                m_entityCount += 1;

                // Send event about created entity.
                this->entityCreated(handleEntry.handle);
            }
            break;

        case EntityCommands::Destroy:
            {
                // Locate the handle entry.
                int handleIndex = command->handle.identifier - 1;
                HandleEntry& handleEntry = m_handles[handleIndex];

                // Check if handles match.
                if(command->handle != handleEntry.handle)
                {
                    // Trying to destroy an entity twice.
                    BOOST_ASSERT(false);
                    continue;
                }

                // Send event about soon to be destroyed entity.
                this->entityDestroyed(handleEntry.handle);

                // Decrement the counter of active entities.
                m_entityCount -= 1;

                // Mark handle flags as free
                BOOST_ASSERT(handleEntry.flags & HandleFlags::Valid);
                BOOST_ASSERT(handleEntry.flags & HandleFlags::Active);
                BOOST_ASSERT(handleEntry.flags & HandleFlags::Destroy);

                handleEntry.flags = HandleFlags::Free;

                // Increment the handle version to invalidate it.
                handleEntry.handle.version += 1;

                // Add the handle entry to the free list queue.
                if(m_freeListIsEmpty)
                {
                    // If there are no elements in the queue,
                    // set the element as the only one in the queue.
                    m_freeListDequeue = handleIndex;
                    m_freeListEnqueue = handleIndex;
                    m_freeListIsEmpty = false;
                }
                else
                {
                    BOOST_ASSERT(m_handles[m_freeListEnqueue].nextFree == InvalidNextFree);

                    // If there are already other elements in the queue,
                    // add the element to the end of the queue chain.
                    m_handles[m_freeListEnqueue].nextFree = handleIndex;
                    m_freeListEnqueue = handleIndex;
                }
            }
            break;
        }
    }

    // Clear processed entity commands.
    m_commands.clear();
}

bool EntitySystem::IsHandleValid(const EntityHandle& entity) const
{
    BOOST_ASSERT(m_initialized);

    // Check if the handle identifier is valid.
    if(entity.identifier <= InvalidIdentifier)
        return false;

    if(entity.identifier > (int)m_handles.size())
        return false;

    // Locate the handle entry.
    int handleIndex = entity.identifier - 1;
    const HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if handle is valid.
    if(!(handleEntry.flags & HandleFlags::Valid))
        return false;

    // Check if handle is scheduled to be destroyed.
    if(handleEntry.flags & HandleFlags::Destroy)
        return false;

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return false;

    return true;
}

unsigned int EntitySystem::GetEntityCount() const
{
    return m_entityCount;
}
