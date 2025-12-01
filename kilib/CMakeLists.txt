if( COMMAND CMAKE_MINIMUM_REQUIRED )
    CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
endif( COMMAND CMAKE_MINIMUM_REQUIRED )

####################################################################################
# The name of the project
####################################################################################

####################################################################################
# The executeable(s) to build to.
####################################################################################
add_library(kilib STATIC "")

target_sources(kilib
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CLayer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CNeuronalNet.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Activation.hpp"

PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CLayer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CNeuronalNet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Activation.cpp"

)

target_link_libraries(kilib PRIVATE utils)

####################################################################################
# Add Unittets if applicable
####################################################################################
if (ENABLE_UNITTESTING)
    enable_testing()
    add_subdirectory(unittests)
endif()

