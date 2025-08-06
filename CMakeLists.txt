# CMakeList.txt : CMake project for NES Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NES_Emulator")

set( SOURCE_PATH "${PROJECT_SOURCE_DIR}/src" )
set( INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include" )

# Path to your SDL2 install
set(SDL2_DIR "C:/dev/SDL2")                # Change this!
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
set(SDL2_LIBRARY_DIR "${SDL2_DIR}/lib/x64")    

file( GLOB_RECURSE INCLUDES "${INCLUDE_PATH}/*.h" )
file( GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp" )

file( GLOB ASSETS "${PROJECT_SOURCE_DIR}/resource/*.*")

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${CPPS} ${INCLUDES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH})

# Add SDL2 includes and libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARY_DIR})

# Link the SDL2 library
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
