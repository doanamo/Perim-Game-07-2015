#pragma once

#include "Precompiled.hpp"

//
// Config
//

namespace System
{
    // Config class.
    class Config
    {
    public:
        Config();
        ~Config();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the class instance.
        bool Initialize();

        // Loads the config from a file.
        bool Load(std::string filename);

        // Sets a config variable.
        template<typename Type>
        void Set(std::string name, const Type& value);
        
        // Gets a config variable. 
        template<typename Type>
        Type Get(std::string name, const Type& default);

    private:
        /*
        // Resolves a reference name.
        LuaRef Resolve(std::string name);
        */

    private:
        // Initialization state.
        bool m_initialized;
    };

    template<typename Type>
    void Config::Set(std::string name, const Type& value)
    {
        if(!m_initialized)
            return;

        /*
        // Resolve reference.
        LuaRef reference = this->Resolve(name);

        // Set new value.
        reference = value;
        */
    }

    template<typename Type>
    Type Config::Get(std::string name, const Type& default)
    {
        if(!m_initialized)
            return default;

        /*
        // Resolve reference.
        LuaRef reference = this->Resolve(name);

        // Get the value.
        try
        {
            return reference.toValue<Type>();
        }
        catch(...)
        {
            return default;
        }
        */

        return default;
    }
};
