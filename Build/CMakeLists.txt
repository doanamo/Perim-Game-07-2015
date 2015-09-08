#
# Build
#

# Minimun version required.
CMake_Minimum_Required(VERSION 3.1.1)

#
# Config
#

# Build settings.
Set(ProjectName "Perim")
Set(TargetName "Game")

# Application settings.
Set(ShowConsole "Yes")

#
# Source
#

# Source files.
Set(SourceDir "../Source")
Set(PrecompiledHeader "Precompiled.hpp")
Set(PrecompiledSource "Precompiled.cpp")

Set(SourceFiles
    "${PrecompiledHeader}"
    "${PrecompiledSource}"

    "Main.cpp"
    "Main.hpp"

    "Common/NonCopyable.hpp"
    "Common/ScopeGuard.hpp"
    "Common/Delegate.hpp"
    "Common/Collector.hpp"
    "Common/Dispatcher.hpp"
    "Common/Receiver.hpp"
    "Common/Context.hpp"
    "Common/Utility.hpp"
    "Common/Utility.cpp"
    "Common/Debug.hpp"
    "Common/Build.hpp"
    "Common/Build.cpp"

    "Logger/Logger.hpp"
    "Logger/Logger.cpp"
    "Logger/Output.hpp"
    "Logger/Message.hpp"
    "Logger/Message.cpp"
    "Logger/Sink.hpp"
    "Logger/Sink.cpp"
    "Logger/FileOutput.hpp"
    "Logger/FileOutput.cpp"
    "Logger/ConsoleOutput.hpp"
    "Logger/ConsoleOutput.cpp"
    "Logger/DebuggerOutput.hpp"
    "Logger/DebuggerOutput.cpp"

    "Lua/State.hpp"
    "Lua/State.cpp"

    "System/Config.hpp"
    "System/Config.cpp"
    "System/Timer.hpp"
    "System/Timer.cpp"
    "System/Window.hpp"
    "System/Window.cpp"
    "System/InputState.hpp"
    "System/InputState.cpp"
    "System/Resource.hpp"
    "System/ResourceManager.hpp"
    "System/ResourceManager.cpp"

    "Graphics/Buffer.hpp"
    "Graphics/Buffer.cpp"
    "Graphics/VertexInput.hpp"
    "Graphics/VertexInput.cpp"
    "Graphics/Texture.hpp"
    "Graphics/Texture.cpp"
    "Graphics/Shader.hpp"
    "Graphics/Shader.cpp"
    "Graphics/ScreenSpace.hpp"
    "Graphics/ScreenSpace.cpp"
    "Graphics/BasicRenderer.hpp"
    "Graphics/BasicRenderer.cpp"
    "Graphics/SpriteSheet.hpp"
    "Graphics/SpriteSheet.cpp"
    "Graphics/AnimationList.hpp"
    "Graphics/AnimationList.cpp"

    "Game/EntityHandle.hpp"
    "Game/EntitySystem.hpp"
    "Game/EntitySystem.cpp"
    "Game/Component.hpp"
    "Game/ComponentSystem.hpp"
    "Game/ComponentSystem.cpp"
    "Game/Components/Transform.hpp"
    "Game/Components/Transform.cpp"
    "Game/Components/Script.hpp"
    "Game/Components/Script.cpp"
    "Game/Components/Render.hpp"
    "Game/Components/Render.cpp"
    "Game/IdentitySystem.hpp"
    "Game/IdentitySystem.cpp"
    "Game/ScriptSystem.hpp"
    "Game/ScriptSystem.cpp"
    "Game/Scripts/Player.hpp"
    "Game/Scripts/Player.cpp"
    "Game/RenderSystem.hpp"
    "Game/RenderSystem.cpp"
)

#
# Project
#

# Create a build project.
Project(${ProjectName})

# Add the source directory as an include directory.
Include_Directories(${SourceDir})

# Enable target folders.
Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)

# Move CMake predefined targets to a separate folder.
Set_Property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Append source directory path to each source file.
Set(SourceFilesTemp)

ForEach(SourceFile ${SourceFiles})
    List(APPEND SourceFilesTemp "${SourceDir}/${SourceFile}")
EndForEach()

Set(SourceFiles ${SourceFilesTemp})

# Organize source files based on their file paths.
ForEach(SourceFile ${SourceFiles})
    # Get the source file directory path.
    Get_Filename_Component(SourceFilePath ${SourceFile} PATH)
    
    # Remove the source directory path.
    String(REPLACE "${SourceDir}" "" SourceFilePath ${SourceFilePath})

    # Add source file to a group.
    If(NOT ${SourceFilePath} STREQUAL "")
        # Replace folder separators.
        String(REPLACE "/" "\\" SourceFilePath ${SourceFilePath})
        
        # Remove leading folder separator.
        String(SUBSTRING ${SourceFilePath} 0 1 FirstCharacter)

        If(FirstCharacter STREQUAL "\\")
            String(LENGTH ${SourceFilePath} SourceFilePathLength)
            Math(EXPR SourceFilePathLength "${SourceFilePathLength} - 1")
            String(SUBSTRING ${SourceFilePath} 1 ${SourceFilePathLength} SourceFilePath)
        EndIf()
    
        # Add a source file to a corresponding group.
        Source_Group("Source\\${SourceFilePath}" FILES ${SourceFile})
    Else()
        # Add source file to the default group.
        Source_Group("Source" FILES ${SourceFile})
    EndIf()
EndForEach()

#
# Target
#

# Create an executable target.
Add_Executable(${TargetName} ${SourceFiles})

# Enable unicode support.
Add_Definitions(-DUNICODE -D_UNICODE)

#
# Build
#

# Write a file in the build directory that will point to the working directory.
Get_Filename_Component(WorkingDir "${CMAKE_CURRENT_LIST_DIR}/../Deploy" ABSOLUTE)
File(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${WorkingDir}/")

# Write a file in the build directory that will point to the source directory.
Get_Filename_Component(SourceDir "${CMAKE_CURRENT_LIST_DIR}/../Source" ABSOLUTE)
File(WRITE "${CMAKE_BINARY_DIR}/SourceDir.txt" "${SourceDir}/")

#
# Windows
#

# Visual C++ compiler.
If("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Show or hide the console window.
    If(ShowConsole)
        Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Console ")
    Else()
        Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Windows ")
    EndIf()
    
    # Restore default main() entry instead of WinMain().
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")

    # Disable Standard C++ Library warnings.
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")
    
    # Use the precompiled header.
    Get_Filename_Component(PrecompiledName ${PrecompiledHeader} NAME_WE)
    
    Set(PrecompiledBinary "$(IntDir)/${PrecompiledName}.pch")
    
    Set_Source_Files_Properties(${SourceFiles} PROPERTIES 
        COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_DEPENDS "${PrecompiledBinary}"
    )
    
    Set_Source_Files_Properties("${SourceDir}/${PrecompiledSource}" PROPERTIES 
        COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_OUTPUTS "${PrecompiledBinary}"
    )
EndIf()

#
# Linux
#

# GCC compiler.
If("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Enable C++11 support.
    List(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
EndIf()

#
# GLM
#

# Add include directory.
Include_Directories("../External/GLM-0.9.6.3")

#
# OpenGL
#

# Find library.
Find_Package(OpenGL REQUIRED)

# Link library.
Target_Link_Libraries(${TargetName} ${OPENGL_gl_LIBRARY})

#
# GLFW
#

# Set build options.
Set(GLFW_BUILD_EXAMPLES OFF)
Set(GLFW_BUILD_TESTS OFF)
Set(GLFW_BUILD_DOCS OFF)
Set(GLFW_INSTALL OFF)

# Add library directory.
Add_Subdirectory("../External/GLFW-3.1.1" "GLFW" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/GLFW-3.1.1/include")

# Set target properties.
Set_Property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
Set_Property(TARGET "glfw" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "glfw")
Target_Link_Libraries(${TargetName} "glfw")

#
# GLEW
#

# Add library directory.
Add_SubDirectory("../External/GLEW-1.12.0" "GLEW" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/GLEW-1.12.0/include")

# Move library target to dependencies.
Set_Property(TARGET "glew32s" PROPERTY PROJECT_LABEL "GLEW")
Set_Property(TARGET "glew32s" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "glew32s")
Target_Link_Libraries(${TargetName} "glew32s")

#
# ZLib
#

# Add library directory.
Add_Subdirectory("../External/ZLib-1.2.8" "ZLib" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/ZLib-1.2.8")
Include_Directories("${CMAKE_BINARY_DIR}/ZLib")

# Move library target to dependencies.
Set_Property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "ZLib")
Set_Property(TARGET "zlibstatic" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "zlibstatic")
Target_Link_Libraries(${TargetName} "zlibstatic")

# Help dependencies find this library.
Set(ZLIB_ROOT "../External/ZLib-1.2.8")
Set(ZLIB_INCLUDE_DIR "../External/ZLib-1.2.8")
Set(ZLIB_LIBRARY "zlibstatic")

#
# PNG
#

# Set build options.
Set(PNG_SHARED OFF)
Set(PNG_STATIC ON)
Set(PNG_TESTS OFF)
Set(SKIP_INSTALL_ALL ON)

# Add library directory.
Add_Subdirectory("../External/PNG-1.6.18" "PNG" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/PNG-1.6.18")
Include_Directories("${CMAKE_BINARY_DIR}/PNG")

# Move library target to dependencies.
Set_Property(TARGET "png16_static" PROPERTY PROJECT_LABEL "PNG")
Set_Property(TARGET "png16_static" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "png16_static")
Target_Link_Libraries(${TargetName} "png16_static")

#
# LuaJIT
#

# Add library directory.
Add_Subdirectory("../External/LuaJIT-2.0.4" "LuaJIT" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/LuaJIT-2.0.4/src")

# Move library targets to dependencies.
Set_Property(TARGET "libluajit" PROPERTY PROJECT_LABEL "LuaJIT")
Set_Property(TARGET "libluajit" PROPERTY FOLDER "External")

Set_Property(TARGET "buildvm" PROPERTY PROJECT_LABEL "LuaJIT-BuildVM")
Set_Property(TARGET "buildvm" PROPERTY FOLDER "External")

Set_Property(TARGET "minilua" PROPERTY PROJECT_LABEL "LuaJIT-MiniLua")
Set_Property(TARGET "minilua" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "libluajit")
Target_Link_Libraries(${TargetName} "libluajit")
