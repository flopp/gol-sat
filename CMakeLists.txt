project(gol-sat)
cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.36.0 COMPONENTS program_options)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "-W -Wall -Werror -std=c++11 -O2")

include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(gol-sat
  src/commandline.cpp
  src/field.cpp
  src/formula.cpp
  src/main.cpp
  src/pattern.cpp
  src/satSolver.cpp
)

include(Minisat)
include_directories(${Boost_INCLUDE_DIRS} ${Minisat_INCLUDE_DIRS})
target_link_libraries(gol-sat ${Boost_LIBRARIES} ${Minisat_LIBRARIES})
add_dependencies(gol-sat MinisatLib)
