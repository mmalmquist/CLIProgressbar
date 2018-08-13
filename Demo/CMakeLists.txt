add_definitions( -std=c89 -Wall -Wextra )
if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
  ADD_DEFINITIONS( -O3 -mtune=native -march=native )
endif ( CMAKE_BUILD_TYPE STREQUAL "Release" )
if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  ADD_DEFINITIONS( -Og -ggdb -Wpedantic )
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

add_executable (demo demo.c)

target_link_libraries (demo LINK_PUBLIC cpb pthread)
