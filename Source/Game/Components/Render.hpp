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

            // Set the diffuse color.
            void SetDiffuseColor(const glm::vec4& color);

            // Gets the transform component.
            Transform* GetTransform();

            // Gets the diffuse color.
            const glm::vec4& GetDiffuseColor() const;

        private:
            // Render parameters.
            glm::vec4 m_diffuseColor;

            // Entity components.
            Transform* m_transform;
        };
    }
}
