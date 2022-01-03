cmake_minimum_required(VERSION 3.8)

###########
# Options #
###########

option(LTO "Enable link-time optimization" OFF)
option(MSVC_LINK_STATIC_RUNTIME "Link the static MSVC runtime library (Visual Studio only)" OFF)
option(PKG_CONFIG_STATIC_LIBS "Link libraries statically" OFF)
option(FORCE_LOCAL_LIBS "Compile libraries locally instead of using CMake to find system installed ones" OFF)

#########
# Setup #
#########

# Define project, source, and includes
project(PaperPup LANGUAGES C CXX)

add_executable(PaperPup WIN32
	# = Engine =
	"src/Main.cpp"
	"src/StreamHelper.h"
	"src/Game.cpp"
	"src/Game.h"
	"src/TimImage.cpp"
	"src/TimImage.h"
	"src/IntArchive.cpp"
	"src/IntArchive.h"
	
	# = System =
	"src/System/System.cpp"
	"src/System/System.h"
	
	"src/System/Frontend/Frontend.h"
	"src/System/Frontend/Frontend_GLFW.cpp"
	"src/System/Frontend/Frontend_GLFW.h"
	
	"src/System/GPU/GPU.h"
	"src/System/GPU/GPU_GL.cpp"
	"src/System/GPU/GPU_GL.h"
	
	"src/System/CD/CD.h"
	"src/System/CD/CD_Standard.cpp"
	"src/System/CD/CD_Standard.h"
	
	"src/System/SPU/SPU.h"
	"src/System/SPU/Mixer.cpp"
	"src/System/SPU/Mixer.h"
	"src/System/SPU/Resampler.h"
	"src/System/SPU/ADPCM.cpp"
	"src/System/SPU/ADPCM.h"
	"src/System/SPU/SPU_miniaudio.cpp"
	"src/System/SPU/SPU_miniaudio.h"
	
	# = Library =
	"lib/glad/glad.c"
	"lib/glad/glad.h"
	"lib/glad/KHR/khrplatform.h"
	"lib/miniaudio.h"
)

target_include_directories(PaperPup PRIVATE "src")
target_include_directories(PaperPup PRIVATE "lib")

##################
# Compiler flags #
##################

# Force Warnings
if(MSVC)
	target_compile_options(PaperPup PRIVATE /W4)
else()
	target_compile_options(PaperPup PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Strip release builds
set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# Statically link other libraries
if(PKG_CONFIG_STATIC_LIBS)
	target_link_options(PaperPup PRIVATE "-static")
endif()

# Use LTO if requested
if(LTO)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT result)
	if(result)
		set_target_properties(PaperPup PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

########
# MSVC #
########

# This is messy as hell, and has been replaced by CMAKE_MSVC_RUNTIME_LIBRARY,
# but that's a very recent CMake addition, so we're still doing it this way for now
if(MSVC AND MSVC_LINK_STATIC_RUNTIME)
	# Statically-link the CRT (vcpkg static libs do this)
	foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		endif()
	endforeach()
endif()

# Do some other MSVC fixes
if(MSVC)
	# Make it so source files are recognized as UTF-8 by MSVC
	target_compile_options(PaperPup PRIVATE "/utf-8")
	
	# Use `main` instead of `WinMain`
	set_target_properties(PaperPup PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

##########################
# Further compiler setup #
##########################

# Force strict C++11
set_target_properties(PaperPup PROPERTIES
	CXX_STANDARD 11
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

# Set debug executable name
set_target_properties(PaperPup PROPERTIES DEBUG_OUTPUT_NAME "PaperPup_debug")

# Set output directory
set(BUILD_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(PaperPup PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BUILD_DIRECTORY}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIRECTORY}
)

################
# Dependencies #
################

# Link GLFW3
if(NOT FORCE_LOCAL_LIBS)
	find_package(glfw3 REQUIRED)
endif()

if(TARGET glfw)
	target_link_libraries(PaperPup PRIVATE glfw)
else()
	set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
	set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
	set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
	set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)
	add_subdirectory("lib/glfw" EXCLUDE_FROM_ALL)
	target_link_libraries(PaperPup PRIVATE glfw)
endif()

# Link GLM
add_subdirectory("lib/glm" EXCLUDE_FROM_ALL)
target_link_libraries(PaperPup PRIVATE glm)

# miniaudio requires libm, libpthread, and libdl
find_library(LIBM m)
if(LIBM)
	target_link_libraries(PaperPup PRIVATE ${LIBM})
endif()

find_library(LIBPTHREAD pthread)
if(LIBPTHREAD)
	target_link_libraries(PaperPup PRIVATE ${LIBPTHREAD})
endif()

target_link_libraries(PaperPup PRIVATE ${CMAKE_DL_LIBS})
