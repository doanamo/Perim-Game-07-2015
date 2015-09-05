#pragma once

#include "Precompiled.hpp"
#include "Logger/Message.hpp"

//
// Logger
//

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Initializes the logger.
    void Initialize();

    // Gets the global logger sink.
    Sink* GetGlobal();
}

// Log macro.
#ifndef NDEBUG
    #define Log() Logger::ScopedMessage(Logger::GetGlobal()).SetSource(__FILE__).SetLine(__LINE__)
#else
    #define Log() Logger::ScopedMessage(Logger::GetGlobal())
#endif
