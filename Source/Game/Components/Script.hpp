#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"
#include "Game/EntityHandle.hpp"

//
// Script Component
//

namespace Game
{
    // Forward declarations.
    class ScriptInterface;

    namespace Components
    {
        // Script component class.
        class Script : public Component
        {
        public:
            // Type declarations.
            typedef std::unique_ptr<ScriptInterface> ScriptPtr;
            typedef std::vector<ScriptPtr> ScriptList;

        public:
            Script();
            ~Script();

            // Adds a script of a given type and returns a pointer to it.
            template<class Type, typename... Arguments>
            Type* Add(Arguments&&... arguments)
            {
                static_assert(std::is_base_of<ScriptInterface, Type>::value, "Not a script type.");
                m_scripts.emplace_back(new Type(std::forward<Arguments>(arguments)...));
                return reinterpret_cast<Type*>(m_scripts.back().get());
            }

            // Updates scripts.
            void Update(EntityHandle self, float timeDelta);

        protected:
            // Finalizes scripts.
            bool Finalize(EntityHandle self, const Context& context) override;

        private:
            // List of scripts.
            ScriptList m_scripts;
        };
    }
}

//
// Script Interface
//

namespace Game
{
    // Script interface class.
    class ScriptInterface
    {
    protected:
        // Protected constructor.
        ScriptInterface()
        {
        }

    public:
        // Virtual destructor.
        virtual ~ScriptInterface()
        {
        }

        // Script logic methods.
        virtual bool OnFinalize(EntityHandle self, const Context& context)
        {
            return true;
        }

        virtual void OnUpdate(EntityHandle self, float timeDelta)
        {
        }
    };
}
