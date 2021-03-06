cmake_minimum_required(VERSION 3.12.4)

# Set up project name and default settings
project(alkahest VERSION 0.0.0.1)
include(cmake/DefaultProjectSettings.cmake)

# Create variable for source files
set(source_dir "${PROJECT_SOURCE_DIR}/src/")
file(
    GLOB
    source_files
    "${source_dir}/init/*.cpp"
    "${source_dir}/net/*.cpp"
    "${source_dir}/ecs/managers/ecsmanager.cpp"
    "${source_dir}/renderer/opengl/*.cpp"
    "${source_dir}/sys/events/*.cpp"
    "${source_dir}/sys/log/*.cpp"
    "${source_dir}/sys/window/*.cpp"
    "${source_dir}/sys/input/*.cpp"
    "${source_dir}/sys/render/*.cpp"
    "${source_dir}/sys/render/opengl/*.cpp"
    "${source_dir}/util/importers/*.cpp"
)

# "Library" for project options / C++ standard
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

# "Library" for compiler warning flags
add_library(project_warnings INTERFACE)

# Add project warnings module
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Add sanitizer module
include(cmake/CompilerSanitizers.cmake)
enable_sanitizers(project_options)

# Add doxygen
include(cmake/Doxygen.cmake)
enable_doxygen(${source_dir})

# Enable static analyzers
include(cmake/StaticAnalysis.cmake)

# Various extra options go here
option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)
option(ENABLE_TESTING "Enable test builds" OFF)

# Configure pre-compiled headers if desired
option(ENABLE_PCH "Enable pre-compiled headers" ON)
if(ENABLE_PCH)
    target_precompile_headers(project_options INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:${PROJECT_SOURCE_DIR}/src/pch.h>"
    )
endif()

# Testing setup
if(ENABLE_TESTING)
    enable_testing()
    message(STATUS "Building tests")
    add_subdirectory(test)
endif()

# For threading support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Check for OpenGL
find_package(OpenGL)

if (OPENGL_FOUND)
	message("OpenGL Correctly Found")
	include_directories(${OPENGL_INCLUDE_DIR})
else (OPENGL_FOUND)
	message("OpenGL environment missing")
endif (OPENGL_FOUND)

# Set lib/ and GLFW/gl3w directories
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GL3W_DIR "${LIB_DIR}/gl3w")
set(GLM_DIR "${LIB_DIR}/glm")
set(FMT_DIR "${LIB_DIR}/fmt")
set(STB_DIR "${LIB_DIR}/stb")
set(ASSIMP_DIR "${LIB_DIR}/assimp")
set(FREETYPE_DIR "${LIB_DIR}/freetype")

# Disable unwanted GLFW features
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

# Set GL_PREFERENCE
set(OpenGL_GL_PREFERENCE GLVND)

# Generate OpenGL profile in build dir
execute_process(COMMAND python3 gl3w_gen.py --root ${PROJECT_BINARY_DIR}/lib/gl3w WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/gl3w)

# Set PIC for fmt as a shared lib
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

# Set options for assimp
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "Build the assimp test suite")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "Build the assimp supplementary tools")

# Build submodules
add_subdirectory(${GLFW_DIR})
add_subdirectory(${GL3W_DIR})
add_subdirectory(${GLM_DIR})
add_subdirectory(${FMT_DIR})
add_subdirectory(${STB_DIR})
add_subdirectory(${ASSIMP_DIR})
add_subdirectory(${FREETYPE_DIR})

# Add executable to build
add_library(alkahest SHARED)
target_sources(alkahest PRIVATE ${source_files})
target_link_libraries(alkahest PRIVATE project_options project_warnings Threads::Threads)

# Library versioning
set_target_properties(alkahest
PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
)

# Add API flags
include(cmake/APIExportFlags.cmake)
set_api_flags(alkahest)

# Set logging level
target_compile_definitions(alkahest PRIVATE LOGGING_LEVEL_ALL)

# Link GLFW
target_link_libraries(alkahest PUBLIC "glfw" "${GLFW_LIBRARIES}")
target_include_directories(alkahest PUBLIC "${GLFW_DIR}/include")
target_compile_definitions(alkahest PUBLIC "GLFW_INCLUDE_NONE")

# Link gl3w source_files
target_link_libraries(alkahest PRIVATE "gl3w")
target_include_directories(alkahest PRIVATE "${GL3W_DIR}/include")

# Link GLM
target_link_libraries(alkahest PUBLIC "glm")
target_include_directories(alkahest PUBLIC "${GLM_DIR}")

# Link fmt
target_link_libraries(alkahest PUBLIC "fmt")
target_include_directories(alkahest PUBLIC "${FMT_DIR}/include")

# Add include directory for STB Image
target_link_libraries(alkahest PUBLIC "stb")
target_include_directories(alkahest PUBLIC "${STB_DIR}")

# Like assimp
target_link_libraries(alkahest PUBLIC "assimp")
target_include_directories(alkahest PUBLIC "${ASSIMP_DIR}/include")

target_link_libraries(alkahest PUBLIC "freetype")
target_include_directories(alkahest PUBLIC "${FREETYPE_DIR}/include")

#
# Post-Build
#

# Copy shader and asset files into build directory
add_custom_command(
    TARGET alkahest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${source_dir}/sys/render/opengl/shaders ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying engine shader files to build directory" VERBATIM
)
add_custom_command(
    TARGET alkahest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying engine asset files to build directory" VERBATIM
)
