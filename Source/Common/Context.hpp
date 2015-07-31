#pragma once

#include "Precompiled.hpp"

//
// Context
//
//  Conveniently holds pointers to instances of different types.
//  Use when you have to pass a non trivial number of references as a single argument.
//

class Context
{
public:
    // Type declarations.
    typedef std::pair<int, boost::any> InstanceHandle;
    typedef std::vector<InstanceHandle> InstanceList;

public:
    Context()
    {
    }

    ~Context()
    {
    }

    // Sets the unique instance.
    template<typename Type>
    bool Set(Type* instance, int index = 0)
    {
        BOOST_ASSERT(index >= 0);

        // Free instance handle if nullptr.
        if(instance == nullptr)
        {
            this->Clear<Type>(index);
        }

        // Find handle by instance entry.
        auto it = std::find_if(m_instances.begin(), m_instances.end(),
            [&index](const InstanceHandle& handle) -> bool
            {
                return handle.first == index && handle.second.type() == typeid(Type*);
            }
        );

        // Set the instance value.
        if(it != m_instances.end())
        {
            // Replace value at existing handle.
            it->second = instance;
            return false;
        }
        else
        {
            // Add a new instance handle.
            m_instances.push_back({ index, instance });
            return true;
        }
    }

    // Gets the unique instance.
    template<typename Type>
    Type* Get(int index = 0) const
    {
        BOOST_ASSERT(index >= 0);

        // Find handle by instance entry.
        auto it = std::find_if(m_instances.begin(), m_instances.end(),
            [&index](const InstanceHandle& handle) -> bool
            {
                return handle.first == index && handle.second.type() == typeid(Type*);
            }
        );

        // Return instance reference.
        if(it != m_instances.end())
        {
            return boost::any_cast<Type*>(it->second);
        }
        else
        {
            return nullptr;
        }
    }

    // Clears the uniqe instance handle.
    template<typename Type>
    void Clear(int index = 0)
    {
        BOOST_ASSERT(index >= 0);

        // Find and erase an instance handle.
        m_instances.erase(std::find_if(m_instances.begin(), m_instances.end(),
            [&index](const InstanceHandle& handle) -> bool
            {
                return handle.first == index && handle.second.type() == typeid(Type*);
            }
        ));
    }

private:
    // List of unique instances.
    InstanceList m_instances;
};
