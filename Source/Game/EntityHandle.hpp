#pragma once

#include "Precompiled.hpp"

//
// Entity Handle
//

namespace Game
{
    // Entity handle structure.
    struct EntityHandle
    {
        // Constructor.
        EntityHandle() :
            identifier(0),
            version(0)
        {
        }

        // Comparison operators.
        bool operator==(const EntityHandle& other) const
        {
            return identifier == other.identifier && version == other.version;
        }

        bool operator!=(const EntityHandle& other) const
        {
            return identifier != other.identifier || version != other.version;
        }

        // Handle data.
        int identifier;
        int version;
    };
}

//
// Handle Hash
//

namespace std
{
    // Entity handle hash functor.
    template<>
    struct hash<Game::EntityHandle>
    {
        std::size_t operator()(const Game::EntityHandle& handle) const
        {
            // Use the identifier as a hash.
            return handle.identifier;
        }
    };

    // Entity handle pair hash functor.
    template<>
    struct hash<std::pair<Game::EntityHandle, Game::EntityHandle>>
    {
        std::size_t operator()(const std::pair<Game::EntityHandle, Game::EntityHandle>& pair) const
        {
            // Use combined identifiers as a hash.
            return pair.first.identifier * std::numeric_limits<int>::max() + pair.second.identifier;
        }
    };
}

namespace boost
{

    // Entity handle hash functor.
    template<>
    struct hash<Game::EntityHandle>
    {
        std::size_t operator()(const Game::EntityHandle& handle) const
        {
            // Use the identifier as a hash.
            return handle.identifier;
        }
    };
}
