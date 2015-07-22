#include "Precompiled.hpp"

void ErrorCallback(int error, const char* description)
{
    Log() << "GLFW Error: " << description;
}

int main(int argc, char* argv[])
{
    // Initialize debug routines.
    Debug::Initialize();

    // Initialize build info.
    Build::Initialize();

    // Initialize logging system.
    Logger::Initialize();

    // Initialize GLFW library.
    glfwSetErrorCallback(ErrorCallback);

    if(!glfwInit())
    {
        Log() << "Failed to initialize GLFW library!";
        return -1;
    }

    BOOST_SCOPE_EXIT(&)
    {
        glfwTerminate();
    };

    return 0;
}
