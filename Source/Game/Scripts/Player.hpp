#pragma once

#include "Precompiled.hpp"
#include "Game/Components/Script.hpp"

// Forward declarations.
namespace System
{
    class InputState;
}

//
// Player
//

namespace Game
{
    namespace Components
    {
        class Transform;
    }

    namespace Scripts
    {
        // Player script class.
        class Player : public ScriptInterface
        {
        public:
            Player();
            ~Player();

            bool OnFinalize(EntityHandle self, const Context& context) override;
            void OnUpdate(EntityHandle self, float timeDelta) override;

        private:
            System::InputState*    m_inputState;
            Components::Transform* m_transform;
        };
    }
}
