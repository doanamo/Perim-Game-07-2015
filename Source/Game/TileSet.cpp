#include "Precompiled.hpp"
#include "TileSet.hpp"
#include "Lua/State.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/SpriteSheet.hpp"
using namespace Game;

namespace
{
    // Log messages.
    #define LogLoadError(filename) "Failed to load a tile set from \"" << filename << "\" file! "

    // Invalid types.
    const TileSet::TileIndex InvalidIndex = 0;
}

TileSet::TileSet(System::ResourceManager* resourceManager) :
    System::Resource(resourceManager)
{
}

TileSet::~TileSet()
{
}

void TileSet::Cleanup()
{
    // Reset texture reference.
    m_texture = nullptr;

    // Clear tile set lists.
    Utility::ClearContainer(m_tiles);
    Utility::ClearContainer(m_names);
}

bool TileSet::Load(std::string filename)
{
    // Restore instance to it's original state.
    this->Cleanup();

    // Setup the cleanup scope guard.
    bool success = false;

    SCOPE_GUARD_IF(!success,
        this->Cleanup());

    // Check if resource manager is bound.
    System::ResourceManager* resourceManager = this->GetResourceManager();

    if(resourceManager == nullptr)
    {
        Log() << LogLoadError(filename) << "Resource manager isn't bound to this instance.";
        return false;
    }

    // Load sprite sheet file.
    Lua::State lua;

    if(!lua.Load(filename))
    {
        Log() << LogLoadError(filename) << "Couldn't load the file.";
        return false;
    }

    // Get the global table.
    lua_getglobal(lua, "TileSet");

    if(!lua_istable(lua, -1))
    {
        Log() << LogLoadError(filename) << "Table \"TileSet\" is missing or invalid.";
        return false;
    }

    // Load the sprite sheet.
    lua_getfield(lua, -1, "SpriteSheet");

    if(!lua_isstring(lua, -1))
    {
        Log() << LogLoadError(filename) << "Field \"TileSet.SpriteSheet\" is missing or invalid.";
        return false;
    }

    auto spriteSheet = resourceManager->Load<Graphics::SpriteSheet>(lua_tostring(lua, -1));

    lua_pop(lua, 1);

    // Hold the sprite sheet texture.
    m_texture = spriteSheet->GetTexture();

    // Read the tiles table.
    lua_getfield(lua, -1, "Tiles");

    if(!lua_istable(lua, -1))
    {
        Log() << LogLoadError(filename) << "Field \"TileSet.Tiles\" is missing or invalid.";
        return false;
    }

    // Declare all tile types first.
    for(lua_pushnil(lua); lua_next(lua, -2); lua_pop(lua, 1))
    {
        // Read the tile name.
        if(!lua_isstring(lua, -2))
        {
            Log() << LogLoadError(filename) << "One of \"TileSet.Tiles\" keys is not a string.";
            return false;
        }

        std::string tileName = lua_tostring(lua, -2);

        // Create a tile type.
        if(!this->CreateTile(tileName))
        {
            Log() << LogLoadError(filename) << "Table \"TileSet.Tiles\" keys must be unique.";
            return false;
        }
    }

    // Check if any tiles were declared.
    if(m_tiles.empty())
    {
        Log() << LogLoadError(filename) << "Tile set data is empty.";
        return false;
    }

    // Read tiles definitions.
    for(lua_pushnil(lua); lua_next(lua, -2); lua_pop(lua, 1))
    {
        TileView defaultTileView;

        // Read the tile name.
        if(!lua_isstring(lua, -2))
        {
            Log() << LogLoadError(filename) << "One of \"TileSet.Tiles\" keys is not a string.";
            return false;
        }

        std::string tileName = lua_tostring(lua, -2);

        // Read the default tile sprite.
        lua_getfield(lua, -1, "Sprite");

        if(!lua_isstring(lua, -2))
        {
            Log() << LogLoadError(filename) << "Field \"TileSet.Tiles[" << tileName << "].Sprite\" is not a string.";
            return false;
        }

        defaultTileView.rectangle = spriteSheet->GetSprite(lua_tostring(lua, -1));

        lua_pop(lua, 1);

        // Read the default tile layer.
        lua_getfield(lua, -1, "Layer");

        if(lua_isnumber(lua, -1))
        {
            defaultTileView.layer = (float)lua_tonumber(lua, -1);
        }

        lua_pop(lua, 1);

        // Read the default tile transparency.
        lua_getfield(lua, -1, "Transparent");

        if(lua_isboolean(lua, -1))
        {
            defaultTileView.transparent = lua_toboolean(lua, -1) == 1 ? true : false;
        }

        lua_pop(lua, 1);

        // Read tile rules.
        lua_getfield(lua, -1, "Rules");

        if(!lua_istable(lua, -1))
        {
            Log() << LogLoadError(filename) << "Field \"TileSet.Tiles[" << tileName << "].Rules\" is not a table.";
            return false;
        }

        for(lua_pushnil(lua); lua_next(lua, -2); lua_pop(lua, 1))
        {
            TileView tileView;
            TileRule tileRule;
        }

        lua_pop(lua, 1);

        // Set default tile view.
        //this->SetDefaultTileView();
    }

    lua_pop(lua, 1);

    // Success!
    return success = true;
}

bool TileSet::CreateTile(std::string name)
{
    // Validate arguments.
    if(name.empty())
    {
        Log() << "Tried to create a tile with an invalid name!";
        return false;
    }

    // Check if a tile with this name already exists.
    auto it = m_names.find(name);

    if(it != m_names.end())
    {
        Log() << "Tile with \"" << name << "\" name already exists within this tile set!";
        return false;
    }

    // Create a new tile.
    m_tiles.emplace_back();
    m_names.emplace(name, m_tiles.size() - 1);

    return true;
}

