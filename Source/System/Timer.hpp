#pragma once

//
// Timer
//

namespace System
{
    // Timer class.
    class Timer
    {
    public:
        Timer();
        ~Timer();

        // Reset instance to it's original state.
        void Cleanup();

        // Calculates frame delta time.
        void Tick();

        // Sets the maximum delta value. 
        void SetMaxDelta(float value);

        // Gets the frame's delta time.
        float GetDelta() const;

    private:
        // Timer values.
        double m_previousTime;

        float m_deltaTime;
        float m_deltaTimeMax;
    };
}
