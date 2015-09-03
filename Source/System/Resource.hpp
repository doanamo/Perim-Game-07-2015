#pragma once

#include "Precompiled.hpp"

//
// Resource
//

namespace System
{
    // Resource base class.
    class Resource : private NonCopyable
    {
    protected:
        Resource()
        {
        }

    public:
        virtual ~Resource()
        {
        }

        virtual void Cleanup() = 0;

        virtual void OnRelease(const std::string& filename)
        {
        }
    };
}
