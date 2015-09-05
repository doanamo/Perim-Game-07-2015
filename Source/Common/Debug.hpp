#pragma once

// Windows specific defines.
#if defined(_WINDOWS) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #define _CRTDBG_MAP_ALLOC_NEW
    #include <stdlib.h>
    #include <crtdbg.h>

    #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define new DEBUG_NEW
#endif

//
// Debug
//

namespace Debug
{
    // Initializes debug routines.
    // Call once at the very beginning of main().
    inline void Initialize()
    {
        // Enable memory leak detection.
        #if defined(_WINDOWS) && !defined(NDEBUG)
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        #endif
    }
}
