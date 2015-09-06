#include "Precompiled.hpp"
#include "State.hpp"
using namespace Lua;

State::State() :
    m_state(nullptr),
    m_initialized(false)
{
}

State::~State()
{
    if(m_initialized)
        this->Cleanup();
}

void State::Cleanup()
{
    // Cleanup Lua state.
    if(m_state != nullptr)
    {
        lua_close(m_state);
        m_state = nullptr;
    }

    // Reset initialization state.
    m_initialized = false;
}

bool State::Initialize()
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD_IF(!m_initialized, 
        this->Cleanup());

    // Create Lua state.
    m_state = luaL_newstate();

    // Success!
    return m_initialized = true;
}

bool State::Load(std::string filename)
{
    // Initialize if needed.
    if(!m_initialized)
    {
        if(!this->Initialize())
            return false;
    }

    // Parse the file.
    if(luaL_dofile(m_state, (Build::GetWorkingDir() + filename).c_str()) != 0)
    {
        Log() << "Lua Error: " << lua_tostring(m_state, -1);
        lua_pop(m_state, 1);
        return false;
    }

    return true;
}

State::operator lua_State*()
{
    return m_state;
}
