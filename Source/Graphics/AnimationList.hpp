#pragma once

#include "Precompiled.hpp"
#include "System/Resource.hpp"

// Forward declarations.
namespace Graphics
{
    class Texture;
}

//
// Animation List
//

namespace Graphics
{
    // Animation list class.
    class AnimationList : public System::Resource
    {
    public:
        // Animation frame.
        struct Frame
        {
            Frame();

            glm::vec4 rectangle;
            glm::vec2 offset;
            float duration;
        };

        // Animation entry.
        struct Animation
        {
            Animation();

            std::vector<Frame> frames;
            float totalDuration;
        };

        // Type declarations.
        typedef std::shared_ptr<const Texture>   TexturePtr;
        typedef std::map<std::string, Animation> AnimationMap;

    public:
        AnimationList(System::ResourceManager* resourceManager);
        ~AnimationList();

        // Restores instance to it's original state.
        void Cleanup();

        // Loads the animation list from a file.
        bool Load(std::string filename);

        // Sets the texture.
        void SetTexture(TexturePtr texture);

        // Gets the texture.
        const TexturePtr& GetTexture() const;

        // Adds an animation.
        bool AddAnimation(std::string name, const std::vector<Frame>& frames);

        // Gets an animation.
        const Animation* GetAnimation(std::string name) const;

    private:
        // Sprite sheet texture.
        TexturePtr m_texture;

        // List of animations.
        AnimationMap m_animations;
    };
}
