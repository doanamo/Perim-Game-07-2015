#pragma once

//
// NonCopyable
//

class NonCopyable
{
protected:
    // Use empty constructor and destructor.
    NonCopyable() { }
    ~NonCopyable() { }

private:
    // Make default copy constructor and operator private.
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};
