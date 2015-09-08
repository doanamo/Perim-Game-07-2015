#include "Precompiled.hpp"
#include "AnimationList.hpp"
#include "Lua/State.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/SpriteSheet.hpp"
using namespace Graphics;

namespace
{
    // Log messages.
    #define LogLoadError(filename) "Failed to load an animation list from \"" << filename << "\" file! "

    // Invalid animation entry.
    const AnimationList::Animation InvalidAnimation;
}

AnimationList::Frame::Frame() :
    rectangle(0.0f, 0.0f, 1.0f, 1.0f),
    offset(0.0f, 0.0f),
    duration(0.0f)
{
}

AnimationList::Animation::Animation() :
    totalDuration(0.0f)
{
}

AnimationList::AnimationList(System::ResourceManager* resourceManager) :
    System::Resource(resourceManager)
{
}

AnimationList::~AnimationList()
{
    this->Cleanup();
}

void AnimationList::Cleanup()
{
    // Reset texture reference.
    m_texture = nullptr;

    // Clear the list of animations.
    Utility::ClearContainer(m_animations);
}

bool AnimationList::Load(std::string filename)
{
    // Restore instance to it's original state.
    this->Cleanup();

    // Setup the cleanup scope guard.
    bool success = false;

    SCOPE_GUARD_IF(!success,
        this->Cleanup());

    // Get the resource manager.
    System::ResourceManager* resourceManager = this->GetResourceManager();

    if(resourceManager == nullptr)
    {
        Log() << LogLoadError(filename) << "Resource manager isn't bound to this instance.";
        return false;
    }

    // Load sprite sheet file.
    Lua::State lua;

    if(!lua.Load(filename))
    {
        Log() << LogLoadError(filename) << "Couldn't load the file.";
        return false;
    }

    // Get the global table.
    lua_getglobal(lua, "AnimationList");

    if(!lua_istable(lua, -1))
    {
        Log() << LogLoadError(filename) << "Table \"AnimationList\" is missing or invalid.";
        return false;
    }

    // Load the sprite sheet.
    lua_getfield(lua, -1, "SpriteSheet");

    if(!lua_isstring(lua, -1))
    {
        Log() << LogLoadError(filename) << "Field \"AnimationList.SpriteSheet\" is missing or invalid.";
        return false;
    }

    auto spriteSheet = resourceManager->Load<SpriteSheet>(lua_tostring(lua, -1));

    lua_pop(lua, 1);

    // Save sprite sheet texture.
    m_texture = spriteSheet->GetTexture();

    // Read the animation table.
    lua_getfield(lua, -1, "Animations");

    if(!lua_istable(lua, -1))
    {
        Log() << LogLoadError(filename) << "Field \"AnimationList.Animations\" is missing or invalid.";
        return false;
    }

    // Iterate over the animation table.
    for(lua_pushnil(lua); lua_next(lua, -2); lua_pop(lua, 1))
    {
        // Check if the key is a string.
        if(!lua_isstring(lua, -2))
        {
            Log() << LogLoadError(filename) << "One of \"AnimationList.Animations\" keys is not a string.";
            return false;
        }

        // Check if the value is a table. 
        if(!lua_istable(lua, -1))
        {
            Log() << LogLoadError(filename) << "One of \"AnimationList.Animations\" values is not a table.";
            return false;
        }

        // Read animation frames.
        std::vector<Frame> frames;

        for(lua_pushnil(lua); lua_next(lua, -2); lua_pop(lua, 1))
        {
            Frame frame;

            // Check if the value is a table. 
            if(!lua_istable(lua, -1))
            {
                Log() << LogLoadError(filename) << "One of \"AnimationList.Animations.Frames\" values is not a table.";
                return false;
            }

            // Read frame sprite.
            lua_getfield(lua, -1, "Sprite");

            if(!lua_isstring(lua, -1))
            {
                Log() << LogLoadError(filename) << "Field \"Animation.Sprite\" is missing or invalid.";
                return false;
            }

            frame.rectangle = spriteSheet->GetSprite(lua_tostring(lua, -1));

            lua_pop(lua, 1);

            // Read frame offset.
            lua_getfield(lua, -1, "Offset");

            if(!lua_istable(lua, -1))
            {
                Log() << LogLoadError(filename) << "Field \"Animation.Offset\" is missing or invalid.";
                return false;
            }

            for(int i = 0; i < 2; ++i)
            {
                lua_pushinteger(lua, i + 1);
                lua_gettable(lua, -2);

                frame.offset[i] = (float)lua_tonumber(lua, -1);

                lua_pop(lua, 1);
            }

            lua_pop(lua, 1);

            // Read frame duration.
            lua_getfield(lua, -1, "Duration");

            if(!lua_isnumber(lua, -1))
            {
                Log() << LogLoadError(filename) << "Field \"Animation.Duration\" is missing or invalid.";
                return false;
            }

            frame.duration = (float)lua_tonumber(lua, -1);

            lua_pop(lua, 1);

            // Add animation frame.
            frames.push_back(frame);
        }

        // Add animation.
        if(!this->AddAnimation(lua_tostring(lua, -2), frames))
        {
            Log() << LogLoadError(filename) << "Couldn't add an animation.";
            return false;
        }
    }

    lua_pop(lua, 1);

    // Success!
    Log() << "Loaded an animation list from \"" << filename << "\" file.";

    return success = true;
}

void AnimationList::SetTexture(TexturePtr texture)
{
    m_texture = texture;
}

const AnimationList::TexturePtr& AnimationList::GetTexture() const
{
    return m_texture;
}

bool AnimationList::AddAnimation(std::string name, const std::vector<Frame>& frames)
{
    if(name.empty())
        return false;

    // Add an animation entry.
    auto result = m_animations.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(name),
        std::forward_as_tuple()
    );

    if(!result.second)
    {
        Log() << "Animation with \"" << name << "\" name already exists within this animation list!";
        return false;
    }

    auto& entry = result.first->second;
    entry.frames = frames;

    // Calculate animation duration.
    for(auto& frame : frames)
    {
        entry.totalDuration += frame.duration;
    }

    return true;
}

const AnimationList::Animation& AnimationList::GetAnimation(std::string name) const
{
    if(name.empty())
        return InvalidAnimation;

    // Find animation by name,
    auto it = m_animations.find(name);

    if(it == m_animations.end())
        return InvalidAnimation;

    return it->second;
}
