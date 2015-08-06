#include "Precompiled.hpp"
#include "Timer.hpp"
using namespace System;

Timer::Timer() :
    m_previousTime(0.0),
    m_deltaTime(0.0),
    m_deltaTimeMax(std::numeric_limits<float>::max())
{
}

Timer::~Timer()
{
}

void Timer::Tick()
{
    // Calculate time delta.
    double currentTime = glfwGetTime();
    m_deltaTime = (float)(currentTime - m_previousTime);
    m_previousTime = currentTime;

    // Sanitize and cap the delta value.
    m_deltaTime = std::max(0.0f, m_deltaTime);
    m_deltaTime = std::min(m_deltaTime, m_deltaTimeMax);
}

void Timer::SetMaxDelta(float value)
{
    m_deltaTimeMax = value;
}

float Timer::GetDelta() const
{
    return m_deltaTime;
}
