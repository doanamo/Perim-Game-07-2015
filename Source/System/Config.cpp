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
    m_context(nullptr),
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
    // Clean config state.
    m_context = LuaContext(nullptr);

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

    // Create base config state.
    try
    {
        // Create a Lua state.
        m_context = LuaContext(false);
        
        // Create a config table.
        m_context.setGlobal("Config", LuaRef::createTable(m_context));
    }
    catch(const LuaException& exception)
    {
        Log() << LogInitializeError() << "An exception occured.";
        Log() << "Lua Error: " << exception.what();
        return false;
    }

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

    // Load the config file.
    try
    {
        m_context.doFile((Build::GetWorkingDir() + filename).c_str());
    }
    catch(const LuaException& exception)
    {
        Log() << LogLoadError(filename) << "An exception occured.";
        Log() << "Lua Error: " << exception.what();
        this->Cleanup();
        return false;
    }
    
    // Success!
    Log() << "Loaded a config from \"" << filename << "\" file.";

    return true;
}

LuaRef Config::Resolve(std::string name)
{
    if(!m_initialized)
        return LuaRef();

    if(name.empty())
        return LuaRef();

    // Get the token that we will start from.
    LuaRef token = m_context.globals().get("Config");

    // Resolve reference.
    auto tokenBegin = name.begin();

    while(true)
    {
        // Get the token ending iterator.
        auto tokenEnd = std::find(tokenBegin, name.end(), '.');

        if(tokenBegin == tokenEnd)
            return LuaRef();

        // Advance token reference deeper into the table.
        token = token[std::string(tokenBegin, tokenEnd)];

        if(token == nullptr)
            return LuaRef();

        // Advance the token beginning iterator.
        if(tokenEnd == name.end())
            break;

        tokenBegin = tokenEnd + 1;
    }

    // Return resolved reference.
    return token;
}
