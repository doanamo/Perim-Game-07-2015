#pragma once

#include "Precompiled.hpp"
#include "Game/Component.hpp"
#include "Graphics/AnimationList.hpp"

//
// Animation Component
//

namespace Game
{
    namespace Components
    {
        // Forward declarations.
        class Render;

        // Animation component class.
        class Animation : public Component
        {
        public:
            // Animation play flags.
            struct PlayFlags
            {
                enum
                {
                    None = 0,
                    Loop = 1 << 0,
                    Reset = 1 << 1,
                    Continue = 1 << 2,
                };

                typedef unsigned int Type;
            };

            // Type declarations.
            typedef std::shared_ptr<const Graphics::AnimationList> AnimationListPtr;

        public:
            Animation();
            ~Animation();

            // Sets the animation list.
            void SetAnimationList(AnimationListPtr animationList);

            // Gets the animation list.
            const AnimationListPtr& GetAnimationList() const;

            // Updates the component.
            void Update(float timeDelta);

            // Plays an animation from the list.
            void Play(std::string name, PlayFlags::Type flags);

            // Stops the current animation.
            void Stop();

            // Checks if an animation is playing.
            bool IsPlaying() const;

        protected:
            // Finalizes component.
            bool Finalize(EntityHandle self, const Context& context) override;

        private:
            // Entity render component.
            Render* m_render;

            // Animation list resource.
            AnimationListPtr m_animationList;

            // Animation state.
            const Graphics::AnimationList::Animation* m_currentAnimation;
            const Graphics::AnimationList::Frame*     m_currentFrame;

            std::size_t m_frameIndex;
            float       m_frameTime;
            bool        m_playing;
            bool        m_loop;
            bool        m_update;
        };
    }
}
