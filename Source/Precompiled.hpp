#pragma once

#include <typeinfo>
#include <typeindex>
#include <limits>
#include <memory>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/scope_exit.hpp>
#include <boost/range.hpp>
#include <boost/any.hpp>
#include <boost/signals2.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLEW_STATIC
#include <gl/glew.h>
#include <glfw/glfw3.h>

#include "Common/Context.hpp"
#include "Common/Debug.hpp"
#include "Common/Build.hpp"
#include "Common/Logger.hpp"
#include "Common/Utility.hpp"
