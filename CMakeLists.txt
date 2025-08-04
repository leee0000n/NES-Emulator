# CMakeList.txt : CMake project for NES Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Define DEBUG if in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

project ("NES_Emulator")

set( SOURCE_PATH "${PROJECT_SOURCE_DIR}/src" )
set( INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include" )

file( GLOB_RECURSE INCLUDES "${INCLUDE_PATH}/*.h" )
file( GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp" )

file( GLOB ASSETS "${PROJECT_SOURCE_DIR}/resource/*.*")

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${CPPS} ${INCLUDES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH})

# TODO: Add tests and install targets if needed.
