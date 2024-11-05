# CMakeList.txt : CMake project for ConsoleTetris, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ConsoleTetris")

# Add source to this project's executable.
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable (ConsoleTetris ${GAME_SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ConsoleTetris PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
