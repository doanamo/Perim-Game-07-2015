#pragma once

#include "Precompiled.hpp"
#include "Game/Components/Script.hpp"
#include "Game/Components/Transform.hpp"

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
    namespace Scripts
    {
        // Player script class.
        class Player : public ScriptInterface
        {
        public:
            Player(System::InputState* inputState, Components::Transform* transform);
            ~Player();

            void OnUpdate(EntityHandle self, float timeDelta) override;

        private:
            System::InputState*    m_inputState;
            Components::Transform* m_transform;
        };
    }
}
