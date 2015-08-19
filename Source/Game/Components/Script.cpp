#include "Precompiled.hpp"
#include "Script.hpp"
using namespace Game;
using namespace Components;

Script::Script()
{
}

Script::~Script()
{
}

bool Script::Finalize(EntityHandle self, const Context& context)
{
    for(auto& script : m_scripts)
    {
        if(!script->OnFinalize(self, context))
            return false;
    }

    return true;
}

void Script::Update(EntityHandle self, float timeDelta)
{
    for(auto& script : m_scripts)
    {
        script->OnUpdate(self, timeDelta);
    }
}
