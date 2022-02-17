# Set default build configuration if none is set
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Debug as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build configuration." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Find ccache
find_program(CCACHE ccache)
if(CCACHE)
    message("Using ccache.")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
    message("ccache not found.")
endif()

# Generate compile_commands for clang-based tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Inter-Procedural Optimization (Link-Time Optimization)
# if it is available on the system / desired by user
option(ENABLE_IPO "Enable Inter-Procedural Optimization (Link-Time Optimization)" OFF)

if(ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(SEND_ERROR "IPO is not supported: ${output}")
    endif()
endif()