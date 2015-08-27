#include "Precompiled.hpp"
#include "ResourceManager.hpp"
using namespace System;

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize the resource manager! "
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    if(m_initialized)
        this->Cleanup();
}

void ResourceManager::Cleanup()
{
    // Remove all resource pools.
    Utility::ClearContainer(m_pools);

    // Reset initialization state.
    m_initialized = false;
}

bool ResourceManager::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    BOOST_SCOPE_EXIT(&)
    {
        if(!m_initialized)
            this->Cleanup();
    };

    // Add instance to the context.
    if(context[ContextTypes::Main].Has<ResourceManager>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Main].Set(this);

    // Success!
    return m_initialized = true;
}
