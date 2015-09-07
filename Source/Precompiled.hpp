#pragma once

#include <cassert>
#include <cctype>
#include <typeinfo>
#include <typeindex>
#include <limits>
#include <memory>
#include <chrono>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

#define GLEW_STATIC
#include <gl/glew.h>
#include <glfw/glfw3.h>

#include <zlib.h>
#include <png.h>

#include <lua.hpp>

#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Delegate.hpp"
#include "Common/Dispatcher.hpp"
#include "Common/Receiver.hpp"
#include "Common/Context.hpp"
#include "Common/Utility.hpp"
#include "Common/Debug.hpp"
#include "Common/Build.hpp"
#include "Logger/Logger.hpp"
#include "Main.hpp"
