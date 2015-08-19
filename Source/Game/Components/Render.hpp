#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"
#include "Game/EntityHandle.hpp"

//
// Render Component
//

namespace Game
{
    namespace Components
    {
        // Forward declarations.
        class Transform;

        // Render component class.
        class Render : public Component
        {
        public:
            Render();
            ~Render();

            // Finalizes render component.
            bool Finalize(EntityHandle self, const Context& context) override;

            // Gets the transform component.
            Transform* GetTransform();

        private:
            Transform* m_transform;
        };
    }
}
