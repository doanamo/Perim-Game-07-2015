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

void Script::OnUpdate(EntityHandle self, float timeDelta)
{
    for(auto& script : m_scripts)
    {
        script->OnUpdate(self, timeDelta);
    }
}
