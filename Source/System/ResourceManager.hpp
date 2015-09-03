#pragma once

#include "Precompiled.hpp"
#include "Resource.hpp"

//
// Resource Manager
//

namespace System
{
    // Resource pool interface.
    class ResourcePoolInterface
    {
    protected:
        ResourcePoolInterface()
        {
        }

    public:
        virtual ~ResourcePoolInterface()
        {
        }

        virtual void ReleaseUnused() = 0;
    };

    // Resource pool class.
    template<typename Type>
    class ResourcePool : public ResourcePoolInterface
    {
    public:
        // Validate resource type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Type declarations.
        typedef std::shared_ptr<Type>                        ResourcePtr;
        typedef std::unordered_map<std::string, ResourcePtr> ResourceList;
        typedef typename ResourceList::value_type            ResourceListPair;
        
    public:
        ResourcePool(std::shared_ptr<const Type> default = nullptr);
        ~ResourcePool();

        // Sets the default resource.
        void SetDefault(std::shared_ptr<const Type> resource);

        // Loads a resource.
        std::shared_ptr<const Type> Load(std::string filename);

        // Releases unused resources.
        void ReleaseUnused();

        // Releases all resources.
        void ReleaseAll();

    private:
        // List of resources.
        ResourceList m_resources;

        // Default resource.
        std::shared_ptr<const Type> m_default;
    };

    template<typename Type>
    ResourcePool<Type>::ResourcePool(std::shared_ptr<const Type> default) :
        m_default(default)
    {
    }

    template<typename Type>
    ResourcePool<Type>::~ResourcePool()
    {
        // Release all resources.
        this->ReleaseAll();
    }

    template<typename Type>
    void ResourcePool<Type>::SetDefault(std::shared_ptr<const Type> resource)
    {
        m_default = resource;
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourcePool<Type>::Load(std::string filename)
    {
        // Find the resource.
        auto it = m_resources.find(filename);

        if(it != m_resources.end())
            return it->second;

        // Load a new resource.
        std::shared_ptr<Type> resource = std::make_shared<Type>();

        if(!resource->Load(filename))
            return m_default;

        // Add resource to the list.
        auto result = m_resources.emplace(filename, std::move(resource));

        assert(result.second == true);

        // Return resource pointer.
        return result.first->second;
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseUnused()
    {
        // Release unused resources.
        auto it = m_resources.begin();

        while(it != m_resources.end())
        {
            auto& resource = it->second;

            if(resource.unique())
            {
                resource->Cleanup();
                resource->OnRelease(it->first);

                it = m_resources.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    template<typename Type>
    void ResourcePool<Type>::ReleaseAll()
    {
        // Release all resources.
        for(auto& pair : m_resources)
        {
            auto& resource = pair.second;

            resource->Cleanup();
            resource->OnRelease(pair.first);
        }

        m_resources.clear();
    }

    // Resource manager class.
    class ResourceManager
    {
    public:
        // Type declarations.
        typedef std::unique_ptr<ResourcePoolInterface>               ResourcePoolPtr;
        typedef std::unordered_map<std::type_index, ResourcePoolPtr> ResourcePoolList;
        typedef ResourcePoolList::value_type                         ResourcePoolPair;

    public:
        ResourceManager();
        ~ResourceManager();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the component system.
        bool Initialize(Context& context);

        // Releases unused resources.
        void ReleaseUnused();

        // Declares a resource type.
        template<typename Type>
        void Declare(std::shared_ptr<const Type> default = nullptr);

        // Loads a resource.
        template<typename Type>
        std::shared_ptr<const Type> Load(std::string filename);

        // Gets a resource pool.
        template<typename Type>
        ResourcePool<Type>* GetPool();

    private:
        // Resource pools.
        ResourcePoolList m_pools;

        // Initialization state.
        bool m_initialized;
    };

    template<typename Type>
    void ResourceManager::Declare(std::shared_ptr<const Type> default)
    {
        if(!m_initialized)
            return;

        // Validate resource type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Check if resource type was already declared.
        auto it = m_pools.find(typeid(Type));

        if(it != m_pools.end())
            return;

        // Create and add pool to the collection.
        auto pool = std::make_unique<ResourcePool<Type>>(default);
        auto pair = ResourcePoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        assert(result.second == true);
    }

    template<typename Type>
    std::shared_ptr<const Type> ResourceManager::Load(std::string filename)
    {
        if(!m_initialized)
            return nullptr;

        // Validate resource type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Get the resource pool.
        ResourcePool<Type>* pool = this->GetPool<Type>();

        if(pool == nullptr)
            return nullptr;

        // Delegate to the resource pool.
        return pool->Load(filename);
    }

    template<typename Type>
    ResourcePool<Type>* ResourceManager::GetPool()
    {
        if(!m_initialized)
            return nullptr;

        // Validate resource type.
        BOOST_STATIC_ASSERT_MSG(std::is_base_of<Resource, Type>::value, "Not a resource type.");

        // Find pool by resource type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
            return nullptr;

        // Cast and return the pointer that we already know is a resource pool.
        return reinterpret_cast<ResourcePool<Type>*>(it->second.get());
    }
};
