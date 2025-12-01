if( COMMAND CMAKE_MINIMUM_REQUIRED )
    CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
endif( COMMAND CMAKE_MINIMUM_REQUIRED )

####################################################################################
# The name of the project
####################################################################################
project(TinyMLTest)
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
# Target OS specific flags.
####################################################################################
if(APPLE)
    # Flags for universal build.
    find_library(Accelerate_Fwk Accelerate)
    mark_as_advanced (Accelerate_Fwk)
    set(EXTRA_LIBS ${Accelerate_Fwk})
    add_compile_definitions(-DACCELERATE_NEW_LAPACK)
else()
    set(EXTRA_LIBS "")
endif(APPLE)

####################################################################################
# The executeable(s) to build to.
####################################################################################
#add_executable(${PROJECT_NAME}
#    "main.cpp"
#)

#target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS} utils kilib)

if(ENABLE_UNITTESTING)
    enable_testing()
endif()

####################################################################################
# Target specific flags.
####################################################################################

add_subdirectory(tsunit)
add_subdirectory(kilib)
add_subdirectory(utils)
add_subdirectory(tools EXCLUDE_FROM_ALL)


