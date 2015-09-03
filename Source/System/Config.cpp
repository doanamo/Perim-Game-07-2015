#include "Precompiled.hpp"
#include "Config.hpp"
using namespace System;

namespace
{
    // Log messages.
    #define LogInitializeError() "Failed to initialize a config instance! "
    #define LogLoadError(filename) "Failed to load a config from \"" << filename << "\" file! "
}

Config::Config() :
    m_lua(nullptr),
    m_initialized(false)
{
}

Config::~Config()
{
    if(m_initialized)
        this->Cleanup();
}

void Config::Cleanup()
{
    // Cleanup Lua state.
    if(m_lua != nullptr)
    {
        lua_close(m_lua);
        m_lua = nullptr;
    }

    // Reset initialization state.
    m_initialized = false;
}

bool Config::Initialize()
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

    // Create Lua state.
    m_lua = luaL_newstate();

    // Success!
    return m_initialized = true;
}

bool Config::Load(std::string filename)
{
    // Initialize the class instance.
    if(!this->Initialize())
    {
        Log() << LogLoadError(filename) << "Couldn't initialize the instance.";
        return false;
    }

    // Setup the cleanup scope guard.
    bool success = false;

    BOOST_SCOPE_EXIT(&)
    {
        if(!success)
            this->Cleanup();
    };

    // Load the config file
    if(luaL_dofile(m_lua, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't parse the file.";
        Log() << "Lua Error: " << lua_tostring(m_lua, -1);
        return false;
    }

    // Validate state.
    lua_getglobal(m_lua, "Config");

    if(!lua_istable(m_lua, -1))
    {
        Log() << LogLoadError(filename) << "Missing \"Config\" table.";
        return false;
    }

    lua_pop(m_lua, 1);
    
    // Success!
    Log() << "Loaded a config from \"" << filename << "\" file.";

    return success = true;
}

void Config::PushReference(std::string name)
{
    assert(m_initialized);

    // Parse name tokens.
    auto tokens = Utility::SplitString(name, '.');

    if(tokens.empty())
    {
        // Push a nil pair.
        lua_pushnil(m_lua);
        lua_pushnil(m_lua);
        return;
    }

    // Push config table pair.
    lua_pushvalue(m_lua, LUA_GLOBALSINDEX);
    lua_pushstring(m_lua, "Config");

    // Traverse the table chain.
    for(const std::string& token : tokens)
    {
        // Get table element.
        lua_gettable(m_lua, -2);

        // Remove previous table.
        lua_remove(m_lua, -2);

        // Check if we got a table.
        if(!lua_istable(m_lua, -1))
        {
            lua_pop(m_lua, 1);

            // Push a nil pair.
            lua_pushnil(m_lua);
            lua_pushnil(m_lua);
            return;
        }

        // Push token key.
        lua_pushstring(m_lua, token.c_str());
    }
}

void Config::PushValue()
{
    assert(m_initialized);

    // Push value and remove table.
    lua_gettable(m_lua, -2);
    lua_remove(m_lua, -2);
}
