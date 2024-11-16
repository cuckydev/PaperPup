cmake_minimum_required(VERSION 3.15)

set(PAPERPUP_VERSION 0.0.0)

project(
	PaperPup
	VERSION ${PAPERPUP_VERSION}
	LANGUAGES C CXX
	HOMEPAGE_URL "https://ckdev.org/"
)

# Compile main binary
add_executable(PaperPup
	"Source/Main.cpp"

	"Source/clownresampler.c"
	"Source/clownresampler.h"

	"Source/Types/Exceptions.h"
	"Source/Types/File.h"
	"Source/Types/GLShader.h"
	"Source/Types/Span.h"
	"Source/Types/Stream.h"
	"Source/Types/UniqueCInstance.h"
	"Source/Types/UniqueGLInstance.h"

	"Source/Util/Endian.h"
	"Source/Util/String.h"
	
	"Source/Script/Thread.cpp"
	"Source/Script/Thread.h"
	"Source/Script/Context.h"

	"Source/Script/Lib.cpp"
	"Source/Script/Lib.h"
	"Source/Script/Lib/Thread.cpp"
	"Source/Script/Lib/Thread.h"

	"Source/Backend/Core.cpp"
	"Source/Backend/Core.h"
	"Source/Backend/Audio.cpp"
	"Source/Backend/Audio.h"
	"Source/Backend/Render.cpp"
	"Source/Backend/Render.h"
	
	"Source/Backend/VFS/VFS.cpp"
	"Source/Backend/VFS.h"
	"Source/Backend/VFS/Path.h"
	"Source/Backend/VFS/DataSource/DataSource.h"
	"Source/Backend/VFS/DataSource/Folder.cpp"
	"Source/Backend/VFS/DataSource/Folder.h"
	
	"Source/PS1/Context.cpp"
	"Source/PS1/Context.h"
	"Source/PS1/Shader.cpp"
	"Source/PS1/Shader.h"

	"Source/PS1/ADPCM.cpp"
	"Source/PS1/ADPCM.h"
	"Source/PS1/INT.cpp"
	"Source/PS1/INT.h"
	"Source/PS1/TIM.cpp"
	"Source/PS1/TIM.h"
	"Source/PS1/TMD.cpp"
	"Source/PS1/TMD.h"
	"Source/PS1/VDF.cpp"
	"Source/PS1/VDF.h"

	"Source/PS1/VDFActor.cpp"
	"Source/PS1/VDFActor.h"
)

target_include_directories(PaperPup PUBLIC "Source")

set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Bin")
set_target_properties(PaperPup PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)

# Compiler setup
add_library(PaperPup.Config INTERFACE)

target_link_libraries(PaperPup PUBLIC PaperPup.Config)

target_compile_definitions(PaperPup.Config INTERFACE PAPERPUP_VERSION="${PAPERPUP_VERSION}")

# Require C++20
target_compile_features(PaperPup.Config INTERFACE cxx_std_20)

# Use UNICODE for Win32 code
target_compile_definitions(PaperPup.Config INTERFACE UNICODE)

# Enable full warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# target_compile_options(PaperPup PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# Remove /W3 from default flags
	string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	target_compile_options(PaperPup.Config INTERFACE /W4)
else()
	target_compile_options(PaperPup.Config INTERFACE -Wall -Wextra)
endif()

# Compile and link dependencies
add_subdirectory("External/glad" EXCLUDE_FROM_ALL)

set(LUAU_BUILD_CLI OFF)
set(LUAU_BUILD_TESTS OFF)
set(LUAU_BUILD_WEB OFF)
add_subdirectory("External/luau" EXCLUDE_FROM_ALL)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory("External/SDL" EXCLUDE_FROM_ALL)

target_link_libraries(PaperPup PUBLIC glad Luau.Compiler Luau.VM SDL3::SDL3-static)

# Generate Luau bindings
set(LEON_BUILD_TESTS OFF)
add_subdirectory("External/Leon" EXCLUDE_FROM_ALL)

target_link_libraries(PaperPup PUBLIC Leon)

leon_target(PaperPup_Leon "${CMAKE_CURRENT_BINARY_DIR}/LeonProject" PaperPup "${CMAKE_CURRENT_SOURCE_DIR}/Leon/Process.lua" ".cpp" ".cpp"
	"Source/Script/Lib/Thread.h"
)
leon_target_outputs(PaperPup_Leon PaperPup)
leon_target_glue(PaperPup_Leon PaperPup)

add_dependencies(PaperPup PaperPup_Leon)
