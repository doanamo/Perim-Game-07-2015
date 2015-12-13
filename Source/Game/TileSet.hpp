#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"

// Forward declarations.
namespace Graphics
{
    class Texture;
}

//
// Tile Set
//

namespace Game
{
    // Tile set class.
    class TileSet : public System::Resource
    {
    public:
        // Tile type index.
        typedef unsigned int TileIndex;

        // Tile view.
        struct TileView
        {
            glm::vec4 rectangle;
            float layer;
            bool transparent;
        };

        // Tile rule logic.
        enum class TileRuleLogic
        {
            Any,
            Is,
            Not,
        };

        // Tile neighbor rule.
        struct TileNeighborRule
        {
            glm::ivec2 offset;

            TileRule rule;
            TileIndex index;
        };

        // Tile rule set.
        struct TileRuleSet
        {
            std::vector<TileNeighborRule> neighborRules;
        };

        struct TileData
        {
            TileView defaultView;
            TileRuleSet rules;
        };

        // Type declarations.
        typedef std::shared_ptr<const Graphics::Texture> TexturePtr;

        typedef std::pair<TileView, TileRuleSet> Tile;
        typedef std::vector<Tile> TileList;
        typedef std::map<std::string, TileIndex> TileLookup;

    public:
        TileSet(System::ResourceManager* resourceManager);
        ~TileSet();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the tile set from a file.
        bool Load(std::string filename);

        // Creates a tile type.
        bool CreateTile(std::string name);



    private:
        // Tile set texture.
        TexturePtr m_texture;

        // Tile set data.
        TileList   m_tiles;
        TileLookup m_names;
    };
}
