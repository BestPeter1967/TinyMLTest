if( COMMAND CMAKE_MINIMUM_REQUIRED )
    CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
endif( COMMAND CMAKE_MINIMUM_REQUIRED )

####################################################################################
# The name of the project
####################################################################################

####################################################################################
# Target specific flags.
####################################################################################
#target_link_libraries(${PROJECT_NAME} PUBLIC pthread)

####################################################################################
# Add support for Tests
####################################################################################
add_library(tsunit STATIC "")

target_sources(tsunit
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/TSUnit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TSUnitTestAddOns.hpp"
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/TSUnit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TSUnitTestAddOns.cpp"
)

target_include_directories(tsunit
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(tsunit
PUBLIC
    UT_USE_COLORED_OUTPUT
)

macro(TESTCASE name)
if (ENABLE_UNITTESTING)
    enable_testing()
    add_executable(UT_${name} ${CMAKE_CURRENT_SOURCE_DIR}/UT_${name}.cpp)
    target_link_libraries(UT_${name} PUBLIC tsunit)
    add_test(NAME UT_${name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/UT_${name})
endif()
endmacro()

# install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)
