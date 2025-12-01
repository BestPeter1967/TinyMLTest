set(CMAKE_CXX_STANDARD 14)
enable_language(C CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_compile_definitions(-DOMIT_CACHE)
if (NOT CMAKE_CROSSCOMPILING)
    set (ENABLE_UNITTESTING ON)
else()
    set (ENABLE_UNITTESTING OFF)
endif()


####################################################################################
# The executeable(s) to build to.
####################################################################################
add_library(utils STATIC "")

target_sources(utils
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CMath.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CVector.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CBLASMathDriver.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CClassicMathDriver.hpp"

PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CMath.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CVector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CBLASMathDriver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CClassicMathDriver.cpp"
)

####################################################################################
# Target specific flags.
####################################################################################

####################################################################################
# Add Unittets if applicable
####################################################################################
if (ENABLE_UNITTESTING)
    enable_testing()
    add_subdirectory(unittests)
endif()

