cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_SOURCE_DIR}/third_party/)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)
include_directories(${CMAKE_SOURCE_DIR}/include/)
add_compile_options(-g -std=c++11)

message(STATUS "platform: ${CMAKE_SYSTEM}")

IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_definitions(-DLINUX)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_definitions(-DWIN32)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
	add_definitions(-DMACOS)
ELSE()
	MESSAGE(FATAL_ERROR "other platform: ${CMAKE_SYSTEM} not surpport")
ENDIF()

add_subdirectory(src)
